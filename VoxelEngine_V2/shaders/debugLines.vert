// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430
#include "_UBO.glsl"

//precision highp float;

layout (location = 0) in vec3 m_position;
layout (location = 3) in vec3 m_color;

//
out fragment_data
{
	vec3 color;

} f_data;

void main()
{
	f_data.color = m_color;
	
	gl_Position = viewProjection * vec4(m_position, 1.0); 
}