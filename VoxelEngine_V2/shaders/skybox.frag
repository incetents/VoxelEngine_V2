// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 uvw;

} f_data;

// Output
layout (location = 0) out vec4 output_color;

// Uniform Textures
layout (binding = 0) uniform samplerCube skybox_handler;

//Main
void main()
{
	output_color = texture(skybox_handler, f_data.uvw);
}