// Copyright(c) 2019 Emmanuel Lajeunesse

// [ Camera Matrices ]
layout (std140, row_major) uniform VXL_Camera_0
{
	mat4 VXL_viewProjection;
	mat4 VXL_view;
	mat4 VXL_projection;
};
vec3 getCameraRight(){		return vec3(-VXL_view[0][0], -VXL_view[1][0], -VXL_view[2][0]);}
vec3 getCameraUp(){			return vec3(-VXL_view[0][1], -VXL_view[1][1], -VXL_view[2][1]);}
vec3 getCameraForwad(){		return vec3(-VXL_view[0][2], -VXL_view[1][2], -VXL_view[2][2]);}
vec3 getCameraPosition(){	return vec3(-VXL_view[0][3], -VXL_view[1][3], -VXL_view[2][3]);}

// [ Time ]
layout (std140) uniform VXL_Time_1
{
	vec4 VXL_time;
	vec4 VXL_sinTime;
	vec4 VXL_cosTime;
};

// [ FBO Size]
layout (std140) uniform VXL_FBOSize_2
{
	vec2 VXL_viewportSize; // Width, Height of bound FBO
	vec2 VXL_texelSize; // 1/Width, 1/Height of bound FBO
};

// [ Core Uniforms for Materials ]
uniform bool  VXL_useModel 			= false;
uniform mat4  VXL_model 			= mat4(1.0);
uniform mat4  VXL_mvp 				= mat4(1.0);
uniform mat3  VXL_normalMatrix	 	= mat3(1.0);
uniform bool  VXL_useInstancing 	= false;
uniform bool  VXL_useTexture 		= false;
uniform vec3  VXL_color 			= vec3(1.0);
uniform vec3  VXL_tint 				= vec3(1.0);
uniform vec4  VXL_viewport 			= vec4(0.0);
uniform float VXL_alpha 			= 1.0;
uniform vec4  VXL_output 			= vec4(1.0);