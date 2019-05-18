// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430
#include "_UBO.glsl"
//precision highp float;

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
uniform bool useMVP = false;
uniform mat4 model = mat4(1.0);

void main()
{
	if(useMVP)
	{
		gl_Position = viewProjection * model * vec4(m_position, 1.0);
		f_data.normal = normalize(vec3(model * vec4(m_normal, 0)));
	}
	else
	{
		gl_Position = vec4(m_position, 1.0);
		f_data.normal = m_normal;
	}

	f_data.pos = gl_Position.xyz;
	f_data.uv = m_uv;
}