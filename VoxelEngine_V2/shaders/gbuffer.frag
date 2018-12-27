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
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_test;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;
// Uniforms
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
	
	output_albedo = texture(albedo_handler, f_data.uv);
	output_normal = vec4(normalize(f_data.normal) * 0.5 + 0.5, 1.0);
	output_test = vec4(d, 0, 0.2, 1);
	
}
//gbuffer gbuffer_vert gbuffer_frag