// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430
#include "_UBO.glsl"

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} f_data;

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_test;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;
// Uniforms
uniform bool VXL_useColorOverride 	= false;
uniform vec3 VXL_color 				= vec3(1,1,1);
uniform vec3 VXL_tint 				= vec3(1,1,1);

uniform int TESTMODE = 0;

//Main
void main()
{
	//gl_FragColor 

	//final_color = vec4(normalize(f_data.normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1);
	//final_color = vec4(f_data.uv.x,f_data.uv.y,1,1);
	vec2 uv = f_data.uv;
	
	if(VXL_useColorOverride)
	{
		output_albedo = vec4(VXL_color * VXL_tint, 1);
	}
	else
	{
		output_albedo = texture(albedo_handler, f_data.uv) * vec4(VXL_tint, 1);
	}
		
	// output UV for testing reasons
	//output_albedo = vec4(f_data.uv, 0, 1);
	
	output_normal = vec4(normalize(f_data.normal) * 0.5 + 0.5, 1.0); // worldspace Normals
	//output_normal = vec4(normalize(f_data.normal), 1.0); // screenspace Normals
	
	if(TESTMODE == 1)
		output_albedo = vec4(normalize(f_data.normal), 1.0);
	if(TESTMODE == 2)
		output_albedo = vec4(normalize(f_data.tangent), 1.0);
	if(TESTMODE == 3)
		output_albedo = vec4(normalize(f_data.bitangent), 1.0);
	
	float d = dot(-getCameraForwad(), normalize(f_data.normal));
	
	output_test = vec4(d, 0, 0.2, 1);
	
	//if(TESTMODE == 1)
	//output_albedo = vec4(d, 0, 0.2, 1);
	
	//output_albedo.r *= red;
	//output_albedo.g *= green;
	//output_albedo.b *= blue;
}
//gbuffer gbuffer_vert gbuffer_frag


