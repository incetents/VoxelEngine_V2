// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} f_data;

// Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Textures
layout (binding = 0) uniform sampler2D texture1;

// Uniforms
uniform vec3 VXL_color = vec3(1,1,1);
uniform float VXL_alpha = 1.0;

//
void main()
{
	float d = dot(normalize(f_data.normal), vec3(0.57735, 0.57735, 0.57735)); // -1 to 1 brightness
	d = d * 0.5 + 0.5; // put into 0-1 range
	vec3 light = vec3(0.25,0.25,0.25);
	vec3 dark = vec3(0.5,0.5,0.5);
		
	output_albedo.rgb = mix(VXL_color - dark, VXL_color + light, d);
	output_albedo.a = VXL_alpha;

	output_normal = vec4(normalize(f_data.normal), 1.0); // worldspace Normals
	
	//output_albedo.rgb = f_data.normal;
}