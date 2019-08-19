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

// Output
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_colorID;

// Textures
layout (binding = 0) uniform sampler2D texture1;

// Uniforms
uniform bool VXL_useTexture = true;
uniform vec3 VXL_color = vec3(1,1,1);
uniform vec4 VXL_output = vec4(0,0,0,0); // used for color ID

//Main
void main()
{
	if(VXL_useTexture)
	{
		vec4 _tex = texture(texture1, f_data.uv);
		output_albedo = _tex;
	}
	else
		output_albedo = vec4(VXL_color, 1);
	
	output_normal = vec4(0,0,1,1);
	output_colorID = VXL_output;
}