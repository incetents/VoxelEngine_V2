// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420
#include "_UBO.glsl"

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;

// Output
out fragment_data
{
	vec2 uv;

} f_data;

// Uniforms
uniform mat4 VXL_model	= mat4(1.0);
uniform vec2 size 		= vec2(1.0,1.0);

// Main
void main()
{
	f_data.uv = m_uv;
	
	mat4 ModelView = view * VXL_model;
	
	// Column 0:
	ModelView[0][0] = size.x;
	ModelView[0][1] = 0;
	ModelView[0][2] = 0;
	
	// Column 1:
	ModelView[1][0] = 0;
	ModelView[1][1] = size.y;
	ModelView[1][2] = 0;
	
	// Column 2:
	ModelView[2][0] = 0;
	ModelView[2][1] = 0;
	ModelView[2][2] = 1;
	
	gl_Position = projection * ModelView * vec4(m_position, 1);
}