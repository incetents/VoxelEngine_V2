// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
//precision highp float;

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;

// Output
out vertex_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} v_data;


// Uniforms

void main()
{
	v_data.pos = m_position;
	v_data.uv = m_uv;
	gl_Position = vec4(m_position, 1.0);
}