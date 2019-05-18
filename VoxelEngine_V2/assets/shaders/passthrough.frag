// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	vec3 normal;

} f_data;

// Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Textures
layout (binding = 0) uniform sampler2D texture1;

// Uniforms
uniform bool useTexture = true;
uniform vec4 color = vec4(1,1,1,1);

uniform bool useFakeLight = false;
uniform vec3 fakeLightDirection = vec3(0.57735, 0.57735, 0.57735);

//Main
void main()
{
	if(useTexture)
	{
		vec4 tex1 = texture(texture1, f_data.uv);
		output_albedo = tex1;
	}
	else if(useFakeLight)
	{
		float d = dot(normalize(f_data.normal), normalize(fakeLightDirection)); // -1 to 1 brightness
		d = d * 0.5 + 0.5; // put into 0-1 range
		vec4 light = vec4(0.25,0.25,0.25,0);
		vec4 dark = vec4(0.5,0.5,0.5,0);
		
		output_albedo = mix(color - dark, color + light, d);
		//output_albedo = color;
		//output_albedo = vec4(f_data.normal, 1);
	}
	else
		output_albedo = color;
		
	output_normal = vec4(0.5,0.5,1,1); // serialized normal (0,0,1) * 0.5 + 0.5
}