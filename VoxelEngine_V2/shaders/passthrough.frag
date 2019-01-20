// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;

} f_data;

//Values sent out (to final array)
layout (location = 0) out vec4 final_color;

layout (binding = 0) uniform sampler2D texture1;

//Main
void main()
{
	vec4 tex1 = texture(texture1, f_data.uv);
	final_color = tex1;
}