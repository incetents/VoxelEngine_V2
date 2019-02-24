// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 color;

} f_data;

//Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

//Main
void main()
{
	output_albedo = vec4(f_data.color, 1);
	output_normal = vec4(0,0,1,1);
}