// Copyright(c) 2019 Emmanuel Lajeunesse

// [ Camera Matrices ]
layout (std140) uniform UBO_Camera_0
{
	mat4 UBO_viewProjection;
	mat4 UBO_view;
	mat4 UBO_projection;
};
vec3 getCameraRight(){return vec3(-UBO_view[0][0], -UBO_view[1][0], -UBO_view[2][0]);}
vec3 getCameraUp(){return vec3(-UBO_view[0][1], -UBO_view[1][1], -UBO_view[2][1]);}
vec3 getCameraForwad(){return vec3(-UBO_view[0][2], -UBO_view[1][2], -UBO_view[2][2]);}
vec3 getCameraPosition(){return vec3(-UBO_view[0][3], -UBO_view[1][3], -UBO_view[2][3]);}

// [ Time ]
layout (std140) uniform UBO_Time_1
{
	vec4 UBO_time;
	vec4 UBO_sinTime;
	vec4 UBO_cosTime;
};

// [ FBO Size]
layout (std140) uniform UBO_FBOSize_2
{
	vec2 UBO_renderSize; // Width, Height of bound FBO
	vec2 UBO_texelSize; // 1/Width, 1/Height of bound FBO
};