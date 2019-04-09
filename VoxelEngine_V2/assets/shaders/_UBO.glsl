// Uniform Blocks used in shaders
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