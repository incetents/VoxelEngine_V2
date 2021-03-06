// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
//precision highp float;

layout (location = 0) in vec3 m_position;
//layout (location = 1) in vec2 m_uv;
//layout (location = 5) in vec3 m_color;


//Values to send to Frag
out vertex_data
{
	vec3 pos;
	//vec2 uv;
	//vec3 color;

} v_data;

void main()
{
	v_data.pos = m_position;
	//v_data.uv = m_uv;
	//v_data.color = m_color;
	gl_Position = vec4(m_position, 1.0);// + Toaster();// + Coaster();
}