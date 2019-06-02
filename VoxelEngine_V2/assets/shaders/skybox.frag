// Copyright(c) 2018 Emmanuel Lajeunesse
#version 420

// Input
in fragment_data
{
	vec3 uvw;

} f_data;

// Output
layout (location = 0) out vec4 output_color;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_test;

// Uniform Textures
layout (binding = 0) uniform samplerCube skybox_handler;

//Main
void main()
{
	//output_color = vec4(f_data.uvw * 0.5 + 0.5, 1);
	output_color = texture(skybox_handler, f_data.uvw);
	output_normal = vec4(0,0,1,1); // worldspace Normals
	output_test = vec4(46.0 / 255.0 , 204.0 / 255.0 , 64.0 / 255.0 , 1);
}