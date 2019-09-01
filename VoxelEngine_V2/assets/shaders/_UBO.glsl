// Copyright(c) 2019 Emmanuel Lajeunesse

// Uniform Blocks used in shaders


// [ Camera Matrices ]
layout (std140) uniform UBO_Camera_0
{
	mat4 viewProjection;
	mat4 view;
	mat4 projection;
};
vec3 getCameraForwad()
{
	return vec3(-view[0][2], -view[1][2], -view[2][2]);
}
vec3 getCameraPosition()
{
	return vec3(-view[0][3], -view[1][3], -view[2][3]);
}

// [ Time ]
layout (std140) uniform UBO_Time_1
{
	vec4 time;
	vec4 sinTime;
	vec4 cosTime;
};

// [ FBO Size]
layout (std140) uniform UBO_FBOSize_2
{
	vec2 renderSize; // Width, Height of bound FBO
	vec2 texelSize; // 1/Width, 1/Height of bound FBO
};