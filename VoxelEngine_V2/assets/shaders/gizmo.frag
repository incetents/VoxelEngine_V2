// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
in vertex_data
{
	//vec3 pos;
	//vec2 uv;
	vec3 normal;
	//vec3 tangent;
	//vec3 bitangent;

} v_data;

// Values sent out (to final array)
layout (location = 0) out vec4 output_albedo;
layout (location = 1) out vec4 output_normal;

// Textures
layout (binding = 0) uniform sampler2D texture1;

// Uniforms
uniform vec3 VXL_color = vec3(1,1,1);
uniform float VXL_alpha = 1.0;
uniform bool ignoreLight = false;
//
void main()
{
	if(ignoreLight)
	{
		output_albedo = vec4(VXL_color, VXL_alpha);
	}
	else
	{
		// -1 to 1 brightness
		float d = dot(normalize(v_data.normal), vec3(0.66725, 0.57193, 0.47661)); // Not perfecty 1,1,1 to have more interesting lighting
		
		// put into 0-1 range
		d = d * 0.5 + 0.5; 
		vec3 light = vec3(0.25,0.25,0.25);
		vec3 dark = vec3(0.5,0.5,0.5); 
			
		output_albedo.rgb = mix(VXL_color - dark, VXL_color + light, d);
		output_albedo.a = VXL_alpha;
	}

	output_normal = vec4(normalize(v_data.normal), 1.0); // worldspace Normals
	
}