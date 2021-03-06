// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in vertex_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} v_data;

// Output
layout (location = 0) out vec4 output_color;

// Uniforms
uniform vec4 VXL_output;

//Main
void main()
{
	output_color = VXL_output;
}