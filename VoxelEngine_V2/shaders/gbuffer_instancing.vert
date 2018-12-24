// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;
layout (location = 2) in vec3 m_normal;
layout (location = 3) in mat4 instanceMatrix;

// Output
out fragment_data
{

	vec3 pos;
	vec2 uv;
	vec3 normal;

} f_data;

// Uniforms
uniform mat4 model			= mat4(1.0);
uniform mat4 viewProjection = mat4(1.0);

// Main
void main()
{
	mat4 model = instanceMatrix * model;
	
	vec4 object_position = model * vec4(m_position, 1.0);

	//object_position.y += 1.2 * gl_InstanceID;
	
	f_data.pos = vec3(object_position);
	f_data.uv = m_uv;
	f_data.normal = vec3(model * vec4(normalize(m_normal), 0));

	gl_Position = viewProjection * object_position; 
}