#version 460 core

uniform vec4 light_diffuse;
uniform vec4 material_diffuse;
uniform vec4 light_specular;
uniform vec4 material_specular;
uniform float material_shininess;
uniform sampler3D texture3D;

in vec4 vertex_color;
in vec4 vertex_normal;
in vec4 vertex_eyevec;
in vec4 vertex_lightdir;
out vec4 FragColor;

void main()
{
	vec4 final_color = vec4(0.1, 0.1, 0.1, 1.0);
	vec4 N = normalize(vertex_normal);
	vec4 L = normalize(vertex_lightdir);
	float lambertTerm = dot(N, -L);

	if (lambertTerm > 0.0)
	{
		final_color += light_diffuse * material_diffuse * lambertTerm;
	}
	FragColor.rgb = final_color.rgb;
	FragColor.a = 1.0;
}