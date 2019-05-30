// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420
#include "_UBO.glsl"

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 6) in vec3 m_tangent;
layout (location = 7) in vec3 m_bitangent;
layout (location = 8) in mat4 instanceMatrix;

// Output
out fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} f_data;

// Uniforms
uniform mat4 VXL_model			= mat4(1.0);
uniform bool VXL_useInstancing 	= false;
uniform bool VXL_useModel 		= true;

// Main
void main()
{
	// Model
	if(VXL_useModel)
	{
		mat4 model = VXL_model;
		
		// Instance matrix occurs before model matrix (if applicable)
		if(VXL_useInstancing)
			model *= instanceMatrix;

		// Position
		f_data.pos = vec3(model * vec4(m_position, 1.0));
		// Normal
		f_data.normal = vec3(model * vec4(m_normal, 0));
		f_data.tangent = vec3(model * vec4(m_tangent, 0));
		f_data.bitangent = vec3(model * vec4(m_bitangent, 0));
	}
	// Passthrough
	else
	{
		// Position
		f_data.pos = m_position;
		// Normal
		f_data.normal = m_normal;
		f_data.tangent = m_tangent;
		f_data.bitangent = m_bitangent;
	}
	
	gl_Position = viewProjection * vec4(f_data.pos, 1); 
	
	// UV
	f_data.uv = m_uv;
	
}