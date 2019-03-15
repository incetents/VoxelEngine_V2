// Copyright(c) 2018 Emmanuel Lajeunesse
#version 430

// Input
in fragment_data
{
	vec3 pos;
	vec2 uv;
	
} f_data;

// Output
layout (location = 0) out vec4 output_color;

// Uniform Textures
layout (binding = 0) uniform sampler2D texture1;
// Uniforms
uniform int outputMode = 0;
uniform float zNear = 1;
uniform float zFar = 1;

// function
float LinearizeDepth(float depth)
{
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

//Main
void main()
{
	vec4 _texture = texture(texture1, f_data.uv);
	//Output Mode
	if(outputMode == 1)
	{
		_texture.rgb = (_texture.rgb) * 2.0 - 1.0; // deserialize
		_texture.rgb = abs(_texture.rgb); // make it visible on both sides
		
		output_color = _texture;
	}
	else if(outputMode == 2)
	{
		float depth = LinearizeDepth(_texture.r);
		output_color = vec4(depth,depth,depth,1);
	}
	else
		output_color = _texture;
	
}













