// Copyright(c) 2018 Emmanuel Lajeunesse
#version 420

// Input
in vec4 v_color;

//Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// uniforms
uniform vec4 _line_color;
uniform vec2 _line_width;

//Main
void main()
{
	output_albedo = v_color;
	output_normal = vec4(0,0,1,1); // worldspace Normals
}