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

uniform sampler2D equirectangular_map;

in vec3 v_position;

out vec4 out_color;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sample_spherical_map(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	return uv * invAtan + 0.5;
}

void main()
{
	vec2 uv = sample_spherical_map(normalize(v_position));
	vec3 color = texture(equirectangular_map, uv).rgb;
	out_color = vec4(color, 1.0);
};