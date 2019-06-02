// Copyright(c) 2018 Emmanuel Lajeunesse
#version 420

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
uniform vec4 VXL_color = vec4(1,1,1,1);

//Main
void main()
{
	float d = dot(normalize(f_data.normal), vec3(0.57735, 0.57735, 0.57735)); // -1 to 1 brightness
	d = d * 0.5 + 0.5; // put into 0-1 range
	vec4 light = vec4(0.25,0.25,0.25,0);
	vec4 dark = vec4(0.5,0.5,0.5,0);
		
	output_albedo = mix(VXL_color - dark, VXL_color + light, d);

	output_normal = vec4(normalize(f_data.normal), 1.0); // worldspace Normals
}