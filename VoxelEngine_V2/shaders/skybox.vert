// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
layout (location = 0) in vec3 m_position;

// Output
out fragment_data
{
	vec3 uvw;

} f_data;

// Uniforms
uniform mat4 VXL_view = mat4(1.0);
uniform mat4 VXL_projection = mat4(1.0);

// Main
void main()
{
	f_data.uvw = m_position;
	
	// Remove position from view matrix since skybox doesn't move
	mat4 viewStill = VXL_view;
	viewStill[3][0] = 0;
	viewStill[3][1] = 0;
	viewStill[3][2] = 0;
	
	vec4 pos = VXL_projection * viewStill * vec4(m_position, 1.0);

	gl_Position = pos.xyww;
}