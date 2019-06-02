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

// Output
layout (location = 0) out vec4 output_color;

// Uniform Textures
layout (binding = 0) uniform sampler2D texture1;
layout (binding = 1) uniform sampler2D texture2;
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
	// Output Mode
	if(outputMode == 1)
	{
		// normal
		_texture.rgb = abs(_texture.rgb); // make it visible on both sides
		
		output_color = _texture;
	}
	else if(outputMode == 2)
	{
		// depth
		float depth = LinearizeDepth(_texture.r);
		output_color = vec4(depth,depth,depth,1);
	}
	else if(outputMode == 3)
	{
		// composite 2 textures
		vec4 _texture2 = texture(texture2, f_data.uv);
		output_color = vec4(mix(_texture.rgb, _texture2.rgb, _texture2.a), 1);
	}
	else // == 0
		// normal
		output_color = _texture;
}













