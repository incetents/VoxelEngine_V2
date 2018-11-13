// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;

} f_data;

// Output
layout (location = 0) out vec4 final_color;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;

//Main
void main()
{
	//final_color = vec4(normalize(f_data.normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1);
	//final_color = vec4(f_data.uv.x,f_data.uv.y,1,1);
	final_color = texture(albedo_handler, f_data.uv);
}