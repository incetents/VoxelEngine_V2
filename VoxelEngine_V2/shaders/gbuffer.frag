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
layout (location = 0) out vec4 final_color1;
layout (location = 1) out vec4 final_color2;
layout (location = 2) out vec4 final_color3;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;

uniform vec3 camForward = vec3(0,0,1);

//Main
void main()
{
	//gl_FragColor 

	//final_color = vec4(normalize(f_data.normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1);
	//final_color = vec4(f_data.uv.x,f_data.uv.y,1,1);
	float d = dot(camForward, normalize(f_data.normal));
	vec3 n = normalize(f_data.normal) * 0.5 + 0.5;
	vec2 uv = f_data.uv;
	
	//final_color1 = vec4(d,0,0.2,1);
	final_color1 = vec4(normalize(f_data.normal) * 0.5 + 0.5, 1.0);
	
	final_color2 = vec4(d, 0, 0.2, 1);
	
	final_color3 = texture(albedo_handler, f_data.uv);
}
//gbuffer gbuffer_vert gbuffer_frag