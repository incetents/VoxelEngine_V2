// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core
#include "_UBO.glsl"

// Geometry Logic
layout(triangles) in; 
layout(line_strip, max_vertices=18) out;
// Converts [Triangle] into [3 Lines]

// Input
in vertex_data
{
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;

} v_data[];

// Output
out vec4 v_color;

// Uniforms
uniform float normal_length = 1.0;

// Main
void drawLine(vec4 pos1, vec4 pos2, vec4 color)
{
	// Normal
	gl_Position = pos1;
	v_color = color;
	EmitVertex();
	
	gl_Position = pos2;
	v_color = color;
	EmitVertex();
	
	EndPrimitive();
}
void main()
{
	int i;
	for(i=0; i< gl_in.length(); i++)
	{
		vec3 P = gl_in[i].gl_Position.xyz;
		vec3 N = v_data[i].normal;
		vec3 T = v_data[i].tangent;
		vec3 B = v_data[i].bitangent;
		
		// Normal
		drawLine(
			viewProjection * vec4(P, 1.0),
			viewProjection * vec4(P + N * normal_length, 1.0),
			vec4(0,0,1,1)
		);
		// Tangent
		drawLine(
			viewProjection * vec4(P, 1.0),
			viewProjection * vec4(P + T * normal_length, 1.0),
			vec4(0,1,0,1)
		);
		// Bitangent
		drawLine(
			viewProjection * vec4(P, 1.0),
			viewProjection * vec4(P + B * normal_length, 1.0),
			vec4(1,0,0,1)
		);
	}
}











