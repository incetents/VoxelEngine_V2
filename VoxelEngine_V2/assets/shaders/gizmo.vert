// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

// Input
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 6) in vec3 m_tangent;
//layout (location = 8) in mat4 instanceMatrix;

// Output
out vertex_data
{
	//vec3 pos;
	//vec2 uv;
	vec3 normal;
	//vec3 tangent;
	//vec3 bitangent;

} v_data;

// Uniforms
uniform mat4 VXL_model			 = mat4(1.0);
uniform mat3 VXL_normalMatrix	 = mat3(1.0);

// Main
void main()
{
	// Position
	mat4 proj = UBO_projection;
	
	//proj[2][2] = 1.0;
	
	gl_Position = proj * UBO_view * VXL_model * vec4(m_position, 1); 

	// Normals
	v_data.normal = VXL_normalMatrix * m_normal;

}