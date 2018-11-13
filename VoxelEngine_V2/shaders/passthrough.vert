#version 420
//precision highp float;

layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_uv;

//
out fragment_data
{
	vec3 pos;
	vec2 uv;

} f_data;
//

void main()
{
	f_data.pos = m_position;
	f_data.uv = m_uv;
	gl_Position = vec4(m_position, 1.0);
}