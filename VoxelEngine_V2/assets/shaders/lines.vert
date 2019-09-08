// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

//precision highp float;

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in float m_width;
layout (location = 3) in vec4 m_color;

// Output
out float vs_width;
out vec4 vs_color;

// Uniforms
uniform bool passthrough = false;

void main()
{
	vs_width = m_width;
	vs_color = m_color;
	
	if(passthrough)
		gl_Position = vec4(m_position, 1.0); 
	else
		gl_Position = UBO_viewProjection * vec4(m_position, 1.0); 
}