// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

//	extern "C"
//	{
//		#include "GL/gl3w.c"
//	}
#include <GL/gl3w.h>
//#include <GL/glcorearb.h>

namespace Vxl
{
	// Data Types
	enum class DataType
	{
		BYTE = GL_BYTE,
		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		HALF_FLOAT = GL_HALF_FLOAT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,
		FIXED = GL_FIXED,
		INT_2_10_10_10_REV = GL_INT_2_10_10_10_REV,
		UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
		UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV
	};
	// Shader Types
	enum class ShaderType
	{
		NONE = 0,
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESSELATION_CONTROL = GL_TESS_CONTROL_SHADER,
		TESSELATION_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	// Uniform Types
	enum class UniformType
	{
		BOOL = GL_BOOL,
		INT = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,
		VEC2 = GL_FLOAT_VEC2,
		VEC3 = GL_FLOAT_VEC3,
		VEC4 = GL_FLOAT_VEC4,
		MAT2 = GL_FLOAT_MAT2,
		MAT3 = GL_FLOAT_MAT3,
		MAT4 = GL_FLOAT_MAT4
	};

	// Buffer Bit Type
	enum class Buffer_Bit_Type
	{
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT
	};
	// Cull Mode
	enum class Cull_Type
	{
		NONE,
		COUNTER_CLOCKWISE,
		CLOCKWISE
	};
	// Blend type
	enum class Blend_Source
	{
		ZERO = GL_ZERO,
		ONE = GL_ONE,
		DST_COLOR = GL_DST_COLOR,
		ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
	};
	enum class Blend_Destination
	{
		ZERO = GL_ZERO,
		ONE = GL_ONE,
		SRC_COLOR = GL_DST_COLOR,
		ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_DST_COLOR,
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
	};
	// Depth Mode (What depth value will overwrite the existing one) 0=you, 1=farthest point
	enum class Depth_Pass_Rule
	{
		LESS = GL_LESS,
		EQUAL = GL_EQUAL,
		GREATER = GL_GREATER,
		LESS_OR_EQUAL = GL_LEQUAL,
		GREATER_OR_EQUAL = GL_GEQUAL,
		NOT_EQUAL = GL_NOTEQUAL,
		ALWAYS = GL_ALWAYS
	};
	// Draw Type
	enum class Draw_Type
	{
		NO_DRAW = -1,
		POINTS = GL_POINTS,								// Geom / Frag	(points)
		LINES = GL_LINES,								// Geom / Frag	(lines)
		LINE_STRIP = GL_LINE_STRIP,						// Geom / Frag	(lines)
		LINE_LOOP = GL_LINE_LOOP,						// Geom / Frag	(lines)
		LINES_ADJACENCY = GL_LINES_ADJACENCY,			// Geom			(lines adjacency)
		LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY, // Geom			(lines adjacency)
		TRIANGLES = GL_TRIANGLES,						// Geom / Frag	(triangles)
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,				// Geom / Frag	(triangles)
		TRIANGLE_FAN = GL_TRIANGLE_FAN,					// Geom / Frag	(triangles)
		TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,	// Geom			(triangles adjacency)
		TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY, // Geom (triangles adjacency)
		PATCHES = GL_PATCHES							// Tesselation Control
	};
	// Draw Mode
	enum class Draw_Mode
	{
		ARRAY,
		ARRAY_INSTANCED,
		INDEXED,
		INDEXED_INSTANCED
	};
	// Buffer Bind Type
	enum class BufferBind_Mode
	{
		STREAM = GL_STREAM_DRAW, // The data store contents will be modified once and used at most a few times.
		STATIC = GL_STATIC_DRAW, // The data store contents will be modified once and used many times.
		DYNAMIC = GL_DYNAMIC_DRAW // The data store contents will be modified repeatedly and used many times.
	};
	// Texture Data
	enum class Texture_Type
	{
		TEX_1D = GL_TEXTURE_1D,
		TEX_2D = GL_TEXTURE_2D,
		TEX_3D = GL_TEXTURE_3D,
		TEX_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
		TEX_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
		TEX_RECT = GL_TEXTURE_RECTANGLE,
		TEX_CUBEMAP = GL_TEXTURE_CUBE_MAP,
		TEX_CUBEMAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
		TEX_BUFFER = GL_TEXTURE_BUFFER,
		TEX_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
		TEX_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};
	enum class Wrap_Mode
	{
		NONE = 0,
		REPEAT = GL_REPEAT,
		CLAMP_BORDER = GL_CLAMP_TO_BORDER,
		CLAMP_STRETCH = GL_CLAMP_TO_EDGE,
		MIRROR_REPEAT = GL_MIRRORED_REPEAT,
		MIRROR_CLAMP = GL_MIRROR_CLAMP_TO_EDGE
	};
	enum class Filter_Mode_Min
	{
		NONE = 0,
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};
	enum class Filter_Mode_Mag
	{
		NONE = 0,
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR
	};
	enum class Format_Count
	{
		R = 1,
		RG = 2,
		RGB = 3,
		RGBA = 4
	};
	enum class Format_Type
	{
		NONE = 0,

		GRAYSCALE = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,

		GRAYSCALE_INT = GL_RED_INTEGER,
		RG_INT = GL_RG_INTEGER,
		RGB_INT = GL_RGB_INTEGER,
		BGR_INT = GL_BGR_INTEGER,
		RGBA_INT = GL_RGBA_INTEGER,
		BGRA_INT = GL_BGRA_INTEGER,

		DEPTH = GL_DEPTH_COMPONENT,
		DEPTH_STENCIL = GL_DEPTH_STENCIL
	};
	const GLenum _Global_Color_Attachment_Calls[16] =
	{
		GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
		GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
	};
	enum class Active_Texture
	{
		LEVEL0 = GL_TEXTURE0,
		LEVEL1 = GL_TEXTURE1,
		LEVEL2 = GL_TEXTURE2,
		LEVEL3 = GL_TEXTURE3,
		LEVEL4 = GL_TEXTURE4,
		LEVEL5 = GL_TEXTURE5,
		LEVEL6 = GL_TEXTURE6,
		LEVEL7 = GL_TEXTURE7,
		LEVEL8 = GL_TEXTURE8,
		LEVEL9 = GL_TEXTURE9,
		LEVEL10 = GL_TEXTURE10,
		LEVEL11 = GL_TEXTURE11,
		LEVEL12 = GL_TEXTURE12,
		LEVEL13 = GL_TEXTURE13,
		LEVEL14 = GL_TEXTURE14,
		LEVEL15 = GL_TEXTURE15,
		LEVEL16 = GL_TEXTURE16,
		LEVEL17 = GL_TEXTURE17,
		LEVEL18 = GL_TEXTURE18,
		LEVEL19 = GL_TEXTURE19,
		LEVEL20 = GL_TEXTURE20,
		LEVEL21 = GL_TEXTURE21,
		LEVEL22 = GL_TEXTURE22,
		LEVEL23 = GL_TEXTURE23,
		LEVEL24 = GL_TEXTURE24,
		LEVEL25 = GL_TEXTURE25,
		LEVEL26 = GL_TEXTURE26,
		LEVEL27 = GL_TEXTURE27,
		LEVEL28 = GL_TEXTURE28,
		LEVEL29 = GL_TEXTURE29,
		LEVEL30 = GL_TEXTURE30,
		LEVEL31 = GL_TEXTURE31,
	};
}