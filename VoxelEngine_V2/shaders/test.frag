#version 430
//precision highp float;

//Values To get from Vertex Shader
in fragment_data
{
	vec3 pos;
	//vec2 uv;
	//vec3 color;

} f_data;

//Values sent out (to final array)
layout (location = 0) out vec4 final_color;

// Uniform Textures
layout (binding = 0) uniform sampler2D albedo_handler;
layout (binding = 1) uniform sampler2D handler1;
layout (binding = 2) uniform sampler2D handler2;

// Uniform Data
layout (location = 0) uniform float value1 = 0.0;
layout (location = 1) uniform float value2 = 0.0;
layout (location = 2) uniform float value3 = 0.0;

/* Uniform Block Types
std140: The packaging of the variables follows rules defined in the OpenGL specification. Blocks with this layout can be shared among shaders.
shared: The storage is implementation dependent, but the compiler will ensure that the block is still shareable among different shaders
packed: The compiler will optimize the block’s storage, possibly removing any variables that are not used in the shader. These type of blocks should not be shared.
*/

layout (std140) uniform orange_2
{
	float bred;
	float bgreen;
	float bblue;
};
layout (std140) uniform farts_1
{
	float ared;
	float agreen;
	float ablue;
};
layout (std140) uniform ColorBlock_0
{
	float red;
	float green;
	float blue;
};

// Uniform Subroutines
subroutine vec4 color_t(); // prototype

// Subroutines
subroutine(color_t)
vec4 ColorRed()
{
	return vec4(1, 0, 0, 1);
}

subroutine(color_t)
vec4 ColorGreen()
{
	return vec4(0, 1, 0, 1);
}

subroutine(color_t)
vec4 ColorBlue()
{
	return vec4(0, 0, 1, 1);
}

layout(location = 0) subroutine uniform color_t Colour1; // pointer to subroutine
layout(location = 1) subroutine uniform color_t Colour2; // pointer to subroutine

// test
subroutine float test_t();
subroutine(test_t)
float t1()
{
	return 1.0;
}
subroutine(test_t)
float t2()
{
	return 2.0;
}

layout(location = 2) subroutine uniform test_t myNum; // pointer to subroutine

// Linearize Depth Buffer Value
//	float Linearize_Depth(float depthSample)
//	{
//	    depthSample = 2.0 * depthSample - 1.0;
//	    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
//	    return zLinear;
//	}


//Main
void main()
{
	//final_color = vec4(1,1,1,1);
	//final_color = vec4(f_data.uv.x, f_data.uv.y, 0.5, 1.0);
	//final_color = vec4(f_data.color, 1);
	final_color = vec4(red,green,blue,1);
	//final_color = vec4(value1,value2,value3,1);
	//final_color = vec4(Colour1() + Colour2());
	//final_color.a = myNum();
	
	//vec4 a = texture(handler1, f_data.uv);
	//vec4 b = texture(handler2, f_data.uv);
	//vec4 c = texture(albedo_handler, f_data.uv);
	//final_color = a/3 + b/3 + c/3;
	//final_color = a/4 + b/4 + c/4 + vec4(value1/1, value2/1, value3/1, 0);
	//final_color /= vec4(255,255,255,255);

	//final_color += Color();
	//final_color += Color2();
	//final_color.a = 1.0;
}