#shader vertex
#version 330 core

in vec3 vtx_position;
in vec2 vtx_texcoord;

out vec2 v_texcoord;

void main()
{
	v_texcoord = vtx_texcoord;
	gl_Position = vec4(vtx_position, 1.0);
};

#shader fragment
#version 330 core

in vec2 v_texcoord;
out vec2 out_color;

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

float ggx_geometry_schlick(float nDotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 2.0;
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

vec2 integrate_brdf(float nDotV, float roughness)
{
	vec3 v = vec3(sqrt(1.0 - nDotV * nDotV), 0.0, nDotV);
	float a = 0.0, b = 0.0;
	vec3 n = vec3(0.0, 0.0, 1.0);

	const uint SAMPLE_COUNT = 1024u;
	for (uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 h = importance_sample_ggx(xi, n, roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);

		float nDotL = max(l.z, 0.0);
		float nDotH = max(h.z, 0.0);
		float vDotH = max(dot(v, h), 0.0);

		if (nDotL > 0.0)
		{
			float g = geometry_smith(n, v, l, roughness);
			float g_vis = (g * vDotH) / (nDotH * nDotV);
			float fc = pow(1.0 - vDotH, 5.0);

			a += (1.0 - fc) * g_vis;
			b += fc * g_vis;
		}
	}
	return vec2(a / float(SAMPLE_COUNT), b / float(SAMPLE_COUNT));
}


void main()
{
	out_color = integrate_brdf(v_texcoord.x, v_texcoord.y);
};