// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430
//precision highp float;

layout (location = 0) in vec3 m_position;
layout (location = 3) in vec3 m_color;

//
out fragment_data
{
	vec3 color;

} f_data;

// Uniforms
uniform mat4 VXL_viewProjection = mat4(1.0);

void main()
{
	f_data.color = m_color;
	
	gl_Position = VXL_viewProjection * vec4(m_position, 1.0); 
}