#shader vertex
#version 330 core

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat3 normal_matrix;

in vec3 vtx_position;
in vec3 vtx_normal;

out vec3 v_position;
out vec3 v_normal;

void main()
{
	v_position = (view_matrix * model_matrix * vec4(vtx_position, 1.0)).xyz;
	v_normal = normalize(normal_matrix * vtx_normal);
	gl_Position = projection_matrix * vec4(v_position, 1.0);
};

#shader fragment
#version 330 core

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

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform DirectionalLight directional_light;
uniform PointLight point_light;
uniform Material material;
uniform vec4 model_color;

in vec3 v_position;
in vec3 v_normal;

out vec4 out_color;


void main()
{
	float gamma = 2.2;
	vec3 view_dir = normalize(-v_position);

	// point light
	vec3 light_dir = normalize(point_light.position.xyz - v_position);
	vec3 halfway_dir = normalize(light_dir + view_dir);
	float distance = length(point_light.position.xyz - v_position);
	float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + point_light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = point_light.ambient * material.ambient;

	// diffuse
	vec3 diffuse = point_light.diffuse * max(dot(v_normal, light_dir), 0.0) * material.diffuse;

	// specular
	vec3 specular = point_light.specular * pow(max(dot(v_normal, halfway_dir), 0.0), material.shininess) * material.specular;
	vec3 lighting = ambient + diffuse + specular;

	// final color
	out_color = vec4(attenuation * pow(lighting, vec3(1.0 / gamma)), 1.0);
};