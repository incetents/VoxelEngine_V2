// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "_UBO.glsl"

// Input
in fragment_data
{
	vec2 uv;

} f_data;

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;

// Uniforms
uniform vec3 VXL_tint = vec3(1,1,1);

// Main
void main()
{
	output_albedo = texture(albedo_handler, f_data.uv);
	
	if(output_albedo.a < 0.01)
		discard;
		
	output_albedo.rgb *= VXL_tint;
	output_normal = vec4(0,0,1,1); // worldspace Normals
}