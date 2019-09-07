// Copyright(c) 2019 Emmanuel Lajeunesse
#version 420 core

// Input
layout (location = 0) in vec2 m_pos;
layout (location = 1) in vec2 m_uv; 

// Output
out vec2 f_uv;

// Uniforms
uniform mat4 VXL_model;

//
void main()
{
    gl_Position = VXL_model * vec4(m_pos.xy, 0.0, 1.0);
    f_uv = m_uv.xy;
}  