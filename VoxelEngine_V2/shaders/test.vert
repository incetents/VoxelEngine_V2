#version 430
//precision highp float;

layout (location = 0) in vec3 m_position;
//layout (location = 1) in vec2 m_uv;
//layout (location = 5) in vec3 m_color;


//Values to send to Frag
out fragment_data
{
	vec3 pos;
	//vec2 uv;
	//vec3 color;

} f_data;

void main()
{
	f_data.pos = m_position;
	//f_data.uv = m_uv;
	//f_data.color = m_color;
	gl_Position = vec4(m_position, 1.0);// + Toaster();// + Coaster();
}