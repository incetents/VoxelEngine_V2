// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
//precision highp float;

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;

// Output
out fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} f_data;


// Uniforms

void main()
{
	f_data.pos = m_position;
	f_data.uv = m_uv;
	gl_Position = vec4(m_position, 1.0);
}