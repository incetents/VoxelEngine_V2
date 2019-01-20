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
uniform vec3 VXL_camForward = vec3(0,0,1);
uniform bool VXL_useColorOverride = false;
uniform vec3 VXL_color = vec3(1,1,1);
uniform vec3 VXL_tint = vec3(1,1,1);

//Main
void main()
{
	//gl_FragColor 

	//final_color = vec4(normalize(f_data.normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1);
	//final_color = vec4(f_data.uv.x,f_data.uv.y,1,1);
	float d = dot(-VXL_camForward, normalize(f_data.normal));
	vec3 n = normalize(f_data.normal) * 0.5 + 0.5;
	vec2 uv = f_data.uv;
	
	if(VXL_useColorOverride)
	{
		output_albedo = vec4(VXL_color * VXL_tint, 1);
	}
	else
	{
		output_albedo = texture(albedo_handler, f_data.uv) * vec4(VXL_tint, 1);
	}
		
	output_normal = vec4(normalize(f_data.normal) * 0.5 + 0.5, 1.0);
	output_test = vec4(d, 0, 0.2, 1);
	
}
//gbuffer gbuffer_vert gbuffer_frag