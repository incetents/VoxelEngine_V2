// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

// Input
layout (location = 0) in vec3 m_position;

// Output
out vertex_data
{
	vec3 uvw;

} v_data;

// Main
void main()
{
	v_data.uvw = m_position;
	
	// Remove position from view matrix since skybox doesn't move
	mat4 viewStill = UBO_view;
	viewStill[3][0] = 0;
	viewStill[3][1] = 0;
	viewStill[3][2] = 0;
	
	vec4 pos = UBO_projection * viewStill * vec4(m_position, 1.0);

	gl_Position = pos.xyww;
}