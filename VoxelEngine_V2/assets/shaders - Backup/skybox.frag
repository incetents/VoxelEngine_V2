// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in fragment_data
{
	vec3 uvw;

} f_data;

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Uniform Textures
layout (binding = 0) uniform samplerCube skybox_handler;

//Main
void main()
{
	//output_color = vec4(f_data.uvw * 0.5 + 0.5, 1);
	output_albedo = texture(skybox_handler, f_data.uvw);
	output_normal = vec4(0,0,1,1); // worldspace Normals
}