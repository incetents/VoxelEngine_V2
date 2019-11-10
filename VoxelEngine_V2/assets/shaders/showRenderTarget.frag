// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "./assets/files/_math.glsl"
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
layout (location = 0) out vec4 output_color;

// Uniform Textures
layout (binding = 0) uniform sampler2D texture1;
layout (binding = 1) uniform sampler2D texture2;
// Uniforms
uniform int outputMode = 0;
uniform int channelOutput = 0;
//uniform float zNear = 1;
//uniform float zFar = 1;

// Puts depth into linear space
float LinearizeDepth(float depth, float zNear, float zFar)
{
    return (2.0 * zNear * zFar) / (zFar + zNear - (2.0 * depth - 1.0) * (zFar - zNear));
}
// Visualize depth buffer
float VisualizeDepth(float depth)
{
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	depth = depth * depth;
	return depth;
}

//Main
void main()
{
	vec4 _texture = texture(texture1, v_data.uv);
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
		float depth = VisualizeDepth(_texture.r);
		output_color = vec4(depth,depth,depth,1);
	}
	else if(outputMode == 3)
	{
		// composite 2 textures
		vec4 _texture2 = texture(texture2, v_data.uv);
		output_color = vec4(mix(_texture.rgb, _texture2.rgb, _texture2.a), 1);
	}
	else if(outputMode == 4)
	{
		// convert to readable HSL regardless of color
		float Hue = dot(_texture.rgb, vec3(8794.0f)); 
		output_color = vec4(HSV_TO_RGB(Hue, 0.75f, 1),1);
	}
	else // == 0
		// normal
		output_color = _texture;
		
	// Final output specialized
	if(channelOutput == 1)
	{
		output_color.rgb = output_color.rrr;
	}
	else if(channelOutput == 2)
	{
		output_color.rgb = output_color.ggg;
	}
	else if(channelOutput == 3)
	{
		output_color.rgb = output_color.bbb;
	}
	else if(channelOutput == 4)
	{
		output_color.rgb = output_color.aaa;
	}
	
	output_color.a = 1.0;

}













