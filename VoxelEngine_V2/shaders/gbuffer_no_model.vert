// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;

// Output
out fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;

} f_data;

// Uniforms
uniform mat4 viewProjection = mat4(1.0);

// Main
void main()
{
	vec4 object_position = vec4(m_position, 1.0);

	f_data.pos = m_position;
	f_data.uv = m_uv;
	f_data.normal = m_normal;

	gl_Position = viewProjection * object_position; 
}