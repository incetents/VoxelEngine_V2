// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

in vec2 f_uv;

// Output
layout (location = 0) out vec4 output_albedo;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;

// Uniform Textures
uniform vec3 VXL_color;

void main()
{    
	float textSample = texture(albedo_handler, f_uv).r;
    output_albedo = vec4(VXL_color, 1.0) * textSample;
}  