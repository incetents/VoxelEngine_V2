#version 420

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;

} f_data;

//Values sent out (to final array)
layout (location = 0) out vec4 final_color;

uniform sampler2D albedo_handler;

//Main
void main()
{
	final_color = texture(albedo_handler, f_data.uv);
}