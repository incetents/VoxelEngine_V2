// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
layout (location = 0) in vec2 m_pos;
layout (location = 1) in vec2 m_uv; 

// Output
out vec2 TexCoords;

// Uniforms
uniform mat4 projection;

//
void main()
{
    gl_Position = projection * vec4(m_pos.xy, 0.0, 1.0);
    TexCoords = m_uv.xy;
}  