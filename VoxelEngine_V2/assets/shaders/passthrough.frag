// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;

} f_data;

// Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Textures
layout (binding = 0) uniform sampler2D texture1;

// Uniforms
uniform bool useTexture = true;
uniform vec4 color = vec4(1,1,1,1);

//Main
void main()
{
	if(useTexture)
	{
		vec4 tex1 = texture(texture1, f_data.uv);
		output_albedo = tex1;
	}
	else
		output_albedo = color;
		
	output_normal = vec4(0.5,0.5,1,1); // serialized normal (0,0,1) * 0.5 + 0.5
}