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

in vec3 v_position;

out vec4 out_color;

const float PI = 3.14159265359;

void main()
{
	vec3 n = normalize(v_position);
	vec3 irradiance = vec3(0.0);

	// tangent space
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, n);
	up = cross(n, right);

	float sample_delta = 0.025;
	float nb_samples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sample_delta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sample_delta)
		{
			// spherical to cartesian
			vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sample_vec = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * n;

			irradiance += texture(environment_map, sample_vec).rgb * cos(theta) * sin(theta);
			nb_samples++;
		}
	}

	irradiance *= PI / nb_samples;
	out_color = vec4(irradiance, 1.0);
};