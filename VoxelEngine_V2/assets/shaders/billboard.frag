// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_UBO.glsl"

// Input
in vertex_data
{
	vec2 uv;

} v_data;

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_reflection;
layout (location = 3) out vec4 output_colorID;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;

// Uniforms
uniform vec3 VXL_tint = vec3(1,1,1);
uniform vec4 VXL_output 	= vec4(0,0,0,0); // used for color ID

//
void main()
{
	output_albedo = texture(albedo_handler, v_data.uv);
	
	if(output_albedo.a < 0.01)
		discard;
		
	output_albedo.rgb *= VXL_tint;
	output_normal = vec4(0,0,1,1); // worldspace Normals
	output_colorID = VXL_output;
}