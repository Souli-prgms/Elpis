#shader vertex
#version 330 core

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

in vec3 vtx_position;

out vec3 v_position;

void main()
{
	v_position = vtx_position;
	gl_Position = projection_matrix * view_matrix * vec4(vtx_position, 1.0);
};

#shader fragment
#version 330 core

uniform samplerCube environment_map;
uniform float roughness;

in vec3 v_position;

out vec4 out_color;

const float PI = 3.14159265359;

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float nDotH = max(dot(n, h), 0.0);

	float denom = (nDotH * nDotH * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a2 / denom;
}

float radical_inverse_vdc(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint n)
{
	return vec2(float(i) / float(n), radical_inverse_vdc(i));
}

vec3 importance_sample_ggx(vec2 xi, vec3 n, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0 * PI * xi.x;
	float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 h = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

	// from tangent-space H vector to world-space sample vector
	vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, n));
	vec3 bitangent = cross(n, tangent);

	vec3 sampleVec = tangent * h.x + bitangent * h.y + n * h.z;
	return normalize(sampleVec);
}

void main()
{
	vec3 n = normalize(v_position);

	// make the simplyfying assumption that V equals R equals the normal 
	vec3 r = n;
	vec3 v = r;

	const uint SAMPLE_COUNT = 1024u;
	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;

	for (uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		// generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 h = importance_sample_ggx(xi, n, roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);

		float nDotL = max(dot(n, l), 0.0);
		if (nDotL > 0.0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float d = distribution_ggx(n, h, roughness);
			float nDotH = max(dot(n, h), 0.0);
			float hDotV = max(dot(h, v), 0.0);
			float pdf = d * nDotH / (4.0 * hDotV) + 0.0001;

			float resolution = 512.0; // resolution of source cubemap (per face)
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			prefilteredColor += textureLod(environment_map, l, mipLevel).rgb * nDotL;
			totalWeight += nDotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;

	out_color = vec4(prefilteredColor, 1.0);
};