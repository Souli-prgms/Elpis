#shader vertex
#version 330 core

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

in vec3 vtx_position;

out vec3 v_position;

void main()
{
	v_position = vtx_position;
	mat4 rot_view = mat4(mat3(view_matrix));
	vec4 clip_pos = projection_matrix * rot_view * vec4(vtx_position, 1.0);
	gl_Position = clip_pos.xyww;
};

#shader fragment
#version 330 core

uniform samplerCube environment_map;

in vec3 v_position;

out vec4 out_color;

void main()
{
	vec3 env_color = texture(environment_map, v_position).rgb;
	env_color = env_color / (env_color + vec3(1.0));
	env_color = pow(env_color, vec3(1.0 / 2.2));
	out_color = vec4(env_color, 1.0);
};