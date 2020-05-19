#shader vertex
#version 330 core

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texcoord;

void main()
{
	v_position = (model_matrix * vec4(vtx_position, 1.0)).xyz;
	v_normal = normalize(mat3(model_matrix) * vtx_normal);
	v_texcoord = vtx_texcoord;
	gl_Position = projection_matrix * view_matrix * vec4(v_position, 1.0);
};

#shader fragment
#version 330 core

#define MAX_NB_LIGHTS 4 

struct DirectionalLight {
	vec4 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float luminosity;
};

struct PointLight {
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float luminosity;
	float constant;
	float linear;
	float quadratic;
};

uniform DirectionalLight directional_light;
uniform PointLight point_lights[MAX_NB_LIGHTS];

uniform vec3 camera_position;

uniform sampler2D albedo_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D normal_map;
uniform float ao;

uniform int nb_lights;
uniform samplerCube irradiance_map;
uniform samplerCube prefilter_map;
uniform sampler2D brdf_map;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 out_color;

const float PI = 3.14159265359;

vec3 normal_from_map()
{
	vec3 tangent_normal = texture(normal_map, v_texcoord).xyz * 2.0 - 1.0;
	vec3 q1 = dFdx(v_position);
	vec3 q2 = dFdy(v_position);
	vec2 st1 = dFdx(v_texcoord);
	vec2 st2 = dFdy(v_texcoord);

	vec3 t = normalize(q1 * st2.t - q2 * st1.t);
	vec3 b = -normalize(cross(v_normal, t));
	mat3 tbn = mat3(t, b, v_normal);
	return normalize(tbn * tangent_normal);
}

float ggx_distribution(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a_2 = a * a;
	float nDotH = max(dot(n, h), 0.0);
	float nDotH_2 = nDotH * nDotH;

	float denom = (nDotH_2 * (a_2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a_2 / denom;
}

float ggx_geometry_schlick(float nDotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	float denom = nDotV * (1.0 - k) + k;

	return nDotV / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float nDotV = max(dot(n, v), 0.0);
	float nDotL = max(dot(n, l), 0.0);
	float ggx2 = ggx_geometry_schlick(nDotV, roughness);
	float ggx1 = ggx_geometry_schlick(nDotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnel_schlick_roughness(float cosTheta, vec3 f0, float roughness)
{
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 v = normalize(camera_position - v_position);
	vec3 n = normal_from_map();
	vec3 r = reflect(-v, n);

	vec3 albedo = pow(texture(albedo_map, v_texcoord).rgb, vec3(2.2));
	float metallic = texture(metallic_map, v_texcoord).r;
	float roughness = texture(roughness_map, v_texcoord).r;

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	for (int i = 0; i < nb_lights; ++i)
	{
		vec3 l = normalize(point_lights[i].position.xyz - v_position);
		vec3 h = normalize(l + v);
		float distance = length(point_lights[i].position.xyz - v_position);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = vec3(300.0, 300.0, 300.0) * attenuation;

		float ndf = ggx_distribution(n, h, roughness);
		float g = geometry_smith(n, v, l, roughness);
		vec3 f = fresnel_schlick(max(dot(h, v), 0.0), f0);

		vec3 ks = f;
		vec3 kd = vec3(1.0) - ks;
		kd *= 1.0 - metallic;

		vec3 num = ndf * g * f;
		float denom = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.001;
		vec3 specular = num / max(denom, 0.001);

		float nDotL = max(dot(n, l), 0.0);
		Lo += (kd * albedo / PI + specular) * radiance * nDotL;
	}

	vec3 f = fresnel_schlick_roughness(max(dot(n, v), 0.0), f0, roughness);
	vec3 ks = f;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0 - metallic;

	vec3 diffuse = texture(irradiance_map, n).rgb * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefiltered_color = textureLod(prefilter_map, r, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdf_map, vec2(max(dot(n, v), 0.0), roughness)).rg;
	vec3 specular = prefiltered_color * (f * brdf.x + brdf.y);

	vec3 ambient = (kd * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	float gamma = 2.2;
	color = pow(color, vec3(1.0 / gamma));
	out_color = vec4(color, 1.0);
};