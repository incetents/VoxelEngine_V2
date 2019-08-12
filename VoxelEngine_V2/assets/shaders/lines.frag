// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in vec4 v_color;

//Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;

// uniforms
uniform vec4 _line_color;
uniform vec2 _line_width;

//Main
void main()
{
	output_albedo = v_color;
}