// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 6) in vec3 m_tangent;
layout (location = 8) in mat4 instanceMatrix;

// Output
out vertex_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} v_data;

// Uniforms
uniform bool VXL_useModel 		 = true;
uniform mat4 VXL_model			 = mat4(1.0);
uniform mat3 VXL_normalMatrix	 = mat3(1.0);
uniform bool VXL_useInstancing 	 = false;

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
		v_data.pos = vec3(model * vec4(m_position, 1.0));
		// Normals
		v_data.normal = VXL_normalMatrix * m_normal;
		v_data.tangent = VXL_normalMatrix * m_tangent;
	}
	// Passthrough
	else
	{
		// Position
		v_data.pos = m_position;
		// Normal
		v_data.normal = m_normal;
		v_data.tangent = m_tangent;
	}
	
	//gl_Position = UBO_viewProjection * vec4(v_data.pos, 1); 
	gl_Position = UBO_view * vec4(v_data.pos, 1); 
	//gl_Position = mat4(1.0) * vec4(v_data.pos, 1); 
	
	// Constants
	v_data.uv = m_uv;
	v_data.bitangent = cross(v_data.normal, v_data.tangent);
}