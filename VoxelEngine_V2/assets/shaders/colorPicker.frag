// Copyright(c) 2019 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} f_data;

// Output
layout (location = 0) out vec4 output_color;

// Uniforms
uniform vec4 colorID;

//Main
void main()
{
	output_color = colorID;
}