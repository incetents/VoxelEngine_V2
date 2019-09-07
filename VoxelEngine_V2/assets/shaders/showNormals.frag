// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in vec4 v_color;

// Output
layout (location = 0) out vec4 output_albedo;

// Uniforms

// Main
void main()
{
	output_albedo = v_color;
}