// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
layout (location = 0) in vec3 m_position;
//layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 6) in vec3 m_tangent;
layout (location = 7) in vec3 m_bitangent;

// Output
out vertex_data
{
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	
} v_data;

// Uniforms
uniform mat4 VXL_model			= mat4(1.0);
uniform bool VXL_useModel 		= true;

// Main
void main()
{
	if(VXL_useModel)
		gl_Position = VXL_model * vec4(m_position, 1);
	else
		gl_Position = vec4(m_position, 1);

	v_data.normal = m_normal;
	v_data.tangent = m_tangent;
	v_data.bitangent = m_bitangent;
}