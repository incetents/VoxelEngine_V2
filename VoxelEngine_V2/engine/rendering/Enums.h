// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <GL/gl3w.h>

namespace Vxl
{
	
	//	// Texture Data
	//	enum class Texture_Type
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		TEX_1D = GL_TEXTURE_1D,
	//		TEX_2D = GL_TEXTURE_2D,
	//		TEX_3D = GL_TEXTURE_3D,
	//		TEX_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
	//		TEX_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
	//		TEX_RECT = GL_TEXTURE_RECTANGLE,
	//		TEX_CUBEMAP = GL_TEXTURE_CUBE_MAP,
	//		TEX_CUBEMAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
	//		TEX_BUFFER = GL_TEXTURE_BUFFER,
	//		TEX_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
	//		TEX_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	//	};
	//	enum class TextureWrapping
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		REPEAT = GL_REPEAT,
	//		CLAMP_BORDER = GL_CLAMP_TO_BORDER,
	//		CLAMP_STRETCH = GL_CLAMP_TO_EDGE,
	//		MIRROR_REPEAT = GL_MIRRORED_REPEAT,
	//		MIRROR_CLAMP = GL_MIRROR_CLAMP_TO_EDGE
	//	};
	//	enum class TextureFilter
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//		
	//		NEAREST = GL_NEAREST,
	//		LINEAR = GL_LINEAR
	//	};
	//	enum class TextureFormat
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		// 1 channel
	//		R8 = GL_R8,
	//		R8_SNORM = GL_R8_SNORM,
	//		R16 = GL_R16,
	//		R16_SNORM = GL_R16_SNORM,
	//		R16F = GL_R16F,
	//		// 2 channels
	//		RG8 = GL_RG8,
	//		RG8_SNORM = GL_RG8_SNORM,
	//		RG16 = GL_RG16,
	//		RG16_SNORM = GL_RG16_SNORM,
	//		RG16F = GL_RG16F,
	//		// 3 channels
	//		RGB8 = GL_RGB8,
	//		RGB8_SNORM = GL_RGB8_SNORM,
	//		RGB16 = GL_RGB16,
	//		RGB16_SNORM = GL_RGB16_SNORM,
	//		RGB16F = GL_RGB16F,
	//		// 4 channels
	//		RGBA8 = GL_RGBA8,
	//		RGBA8_SNORM = GL_RGBA8_SNORM,
	//		RGBA16 = GL_RGBA16,
	//		RGBA16_SNORM = GL_RGBA16_SNORM,
	//		RGBA16F = GL_RGBA16F,
	//		R11F_G11F_B10F = GL_R11F_G11F_B10F,// GOOD FOR GBUFFER
	//	
	//		// [ Special ]
	//		SRGB8 = GL_SRGB8,
	//		SRGBA8 = GL_SRGB8_ALPHA8,
	//	
	//		// [DepthTextureFormat]
	//		STENCIL8 = GL_STENCIL_INDEX8,
	//		DEPTH16 = GL_DEPTH_COMPONENT16,
	//		DEPTH24 = GL_DEPTH_COMPONENT24,
	//		DEPTH32 = GL_DEPTH_COMPONENT32,
	//		DEPTH32F = GL_DEPTH_COMPONENT32F,
	//		DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
	//		DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8
	//	};
	//	enum class DepthTextureFormat
	//	{
	//		STENCIL8 = GL_STENCIL_INDEX8,
	//		DEPTH16 = GL_DEPTH_COMPONENT16,
	//		DEPTH24 = GL_DEPTH_COMPONENT24,
	//		DEPTH32 = GL_DEPTH_COMPONENT32,
	//		DEPTH32F = GL_DEPTH_COMPONENT32F,
	//		DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
	//		DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8
	//	};
	//	enum class TextureChannelType
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		R = GL_RED,		// 1 channel
	//		RG = GL_RG,		// 2 channels
	//		RGB = GL_RGB,	// 3 channels
	//		RGBA = GL_RGBA,	// 4 channels
	//	
	//		BGR = GL_BGR,	// 3 channels
	//		BGRA = GL_BGRA, // 4 channels
	//	
	//		STENCIL = GL_STENCIL_INDEX, // ? channels 
	//		DEPTH = GL_DEPTH_COMPONENT, // ? channels
	//		DEPTH_STENCIL = GL_DEPTH_STENCIL, // ? channels
	//	};
	//	enum class TexturePixelType
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	//		BYTE = GL_BYTE,
	//		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	//		SHORT = GL_SHORT,
	//		UNSIGNED_INT = GL_UNSIGNED_INT,
	//		INT = GL_INT,
	//		HALF_FLOAT = GL_HALF_FLOAT,
	//		FLOAT = GL_FLOAT,
	//		UNSIGNED_BYTE_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
	//		UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
	//		UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
	//		UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
	//		UNSIGNED_INT_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
	//		UNSIGNED_INT_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
	//	};
	//	const GLenum _Global_Color_Attachment_Calls[16] =
	//	{
	//		GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,
	//		GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
	//		GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,
	//		GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
	//		GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,
	//		GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
	//		GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
	//		GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
	//	};
	//	enum class ActiveTexture
	//	{
	//		NONE = -1, // Error (Used for placeholder)
	//	
	//		LEVEL0 = GL_TEXTURE0,
	//		LEVEL1 = GL_TEXTURE1,
	//		LEVEL2 = GL_TEXTURE2,
	//		LEVEL3 = GL_TEXTURE3,
	//		LEVEL4 = GL_TEXTURE4,
	//		LEVEL5 = GL_TEXTURE5,
	//		LEVEL6 = GL_TEXTURE6,
	//		LEVEL7 = GL_TEXTURE7,
	//		LEVEL8 = GL_TEXTURE8,
	//		LEVEL9 = GL_TEXTURE9,
	//		LEVEL10 = GL_TEXTURE10,
	//		LEVEL11 = GL_TEXTURE11,
	//		LEVEL12 = GL_TEXTURE12,
	//		LEVEL13 = GL_TEXTURE13,
	//		LEVEL14 = GL_TEXTURE14,
	//		LEVEL15 = GL_TEXTURE15,
	//		LEVEL16 = GL_TEXTURE16,
	//		LEVEL17 = GL_TEXTURE17,
	//		LEVEL18 = GL_TEXTURE18,
	//		LEVEL19 = GL_TEXTURE19,
	//		LEVEL20 = GL_TEXTURE20,
	//		LEVEL21 = GL_TEXTURE21,
	//		LEVEL22 = GL_TEXTURE22,
	//		LEVEL23 = GL_TEXTURE23,
	//		LEVEL24 = GL_TEXTURE24,
	//		LEVEL25 = GL_TEXTURE25,
	//		LEVEL26 = GL_TEXTURE26,
	//		LEVEL27 = GL_TEXTURE27,
	//		LEVEL28 = GL_TEXTURE28,
	//		LEVEL29 = GL_TEXTURE29,
	//		LEVEL30 = GL_TEXTURE30,
	//		LEVEL31 = GL_TEXTURE31,
	//	};
}