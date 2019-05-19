// Copyright(c) 2018 Emmanuel Lajeunesse
#version 420
#include "_UBO.glsl"

//precision highp float;

layout (location = 0) in vec3 m_position;
layout (location = 1) in float m_width;
layout (location = 3) in vec4 m_color;

//
out float vs_width;
out vec4 vs_color;

void main()
{
	vs_width = m_width;
	vs_color = m_color;
	
	gl_Position = viewProjection * vec4(m_position, 1.0); 
}