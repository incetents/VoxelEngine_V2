// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

// Input
in vertex_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} v_data;

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_reflection;
layout (location = 3) out vec4 output_colorID;
// colorID, emissions, metallicness, roughness

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;
// Uniforms
uniform bool VXL_useTexture = false;
uniform vec3 VXL_color 		= vec3(1,1,1);
uniform vec3 VXL_tint 		= vec3(1,1,1);
uniform vec4 VXL_output 	= vec4(0,0,0,0); // used for color ID
uniform float VXL_alpha 	= 1.0;

//Main
void main()
{
	//gl_FragColor 

	//final_color = vec4(normalize(v_data.normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1);
	//final_color = vec4(v_data.uv.x,v_data.uv.y,1,1);
	vec2 uv = v_data.uv;
	
	if(VXL_useTexture)
	{
		vec4 _tex = texture(albedo_handler, v_data.uv);
		output_albedo.rgb = _tex.rgb * VXL_tint;
	}
	else
	{
		output_albedo.rgb = VXL_color * VXL_tint;
	}
		
	// output UV for testing reasons
	//output_albedo = vec4(v_data.uv, 0, 1);
	
	output_normal = vec4(normalize(v_data.normal), 1); // worldspace Normals
	
	//	if(TESTMODE == 1)
	//		output_albedo = vec4(normalize(v_data.normal), 1.0);
	//	if(TESTMODE == 2)
	//		output_albedo = vec4(normalize(v_data.tangent), 1.0);
	//	if(TESTMODE == 3)
	//		output_albedo = vec4(normalize(v_data.bitangent), 1.0);
	
	output_albedo.a = VXL_alpha;

	//float d = dot(-getCameraForwad(), normalize(v_data.normal));	
	//output_test = vec4(d, 0, 0.2, 1);

	output_reflection = vec4(0,0,0,1);
	output_colorID = VXL_output;
	
	//if(TESTMODE == 1)
	//output_albedo = vec4(d, 0, 0.2, 1);
	
	//output_albedo.r *= red;
	//output_albedo.g *= green;
	//output_albedo.b *= blue;
}
//gbuffer gbuffer_vert gbuffer_frag


