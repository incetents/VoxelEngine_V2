// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Graphics.h"

#include <GL/gl3w.h>

#include "FramebufferObject.h"
#include "Shader.h"
#include "RenderBuffer.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"

#include "../math/Vector.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include <Psapi.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

namespace Vxl
{
	// Global Variables //
	int Graphics::GLVersionMajor = -1;
	int Graphics::GLVersionMinor = -1;
	int Graphics::GLObjectNameMaxLength = -1;
	float Graphics::GLMaxAnisotropy = -1;
	int Graphics::GLMaxFBOColorAttachments = -1;
	int Graphics::GLMaxUniformBindings = -1;
	int Graphics::GLMaxAttributes = -1;

	std::string Graphics::Gpu_Renderer;
	std::string Graphics::Gpu_OpenGLVersion;
	std::string Graphics::Gpu_Vendor;
	VendorType Graphics::Vendor = VendorType::UNKNOWN;

	int Graphics::VRAM_Maximum_KB = -1;
	int Graphics::VRAM_Current_KB = -1;

	uint64_t Graphics::CPU::VirtualMemory::Total_KB = -1;
	uint64_t Graphics::CPU::VirtualMemory::Used_KB = -1;
	uint64_t Graphics::CPU::VirtualMemory::UsedByProcess_KB = -1;

	uint64_t Graphics::CPU::RAM::Total_KB = -1;
	uint64_t Graphics::CPU::RAM::Used_KB = -1;
	uint64_t Graphics::CPU::RAM::UsedByProcess_KB = -1;

	bool Graphics::UsingErrorCallback = false;

	

	// Tracking GL States //
	CullMode			gl_cullmode = CullMode::NONE;
	bool				gl_blendState = false;
	bool				gl_depthRead = false;
	bool				gl_depthWrite = false;
	bool				gl_wireframe = false;
	bool				gl_seamlessCubemaps = false;
	BlendSource			gl_blendsrc = BlendSource::NONE;
	BlendDestination	gl_blenddest = BlendDestination::NONE;
	BlendEquation		gl_blendequation = BlendEquation::NONE;
	DepthPassRule		gl_depthpassrule = DepthPassRule::NONE;
	GLsizei				gl_viewport[4] = { -1, -1, -1, -1 };
	TextureID			gl_activeTextureId = 0;
	RenderBufferID		gl_activeBufferId = 0;
	TextureLevel		gl_activeTextureLayer = TextureLevel::NONE;
	FramebufferObjectID gl_activeFBO = 0;
	ShaderProgramID		gl_activeShaderProgram = 0;
	VBOID				gl_activeVAO = 0;
	float				gl_lineWidth = 1.0f;
	PixelAlignment		gl_pixelPackAlignment = PixelAlignment::ALIGN_4;
	PixelAlignment		gl_pixelUnpackAlignment = PixelAlignment::ALIGN_4;

	// ~ Graphics Enums ~ //
	const int GL_ObjectType[] =
	{
		-1, // Error (Used for placeholder)

		GL_BUFFER,
		GL_SHADER,
		GL_PROGRAM,
		GL_VERTEX_ARRAY,
		GL_QUERY,
		GL_PROGRAM_PIPELINE,
		GL_TRANSFORM_FEEDBACK,
		GL_SAMPLER,
		GL_TEXTURE,
		GL_RENDERBUFFER,
		GL_FRAMEBUFFER
	};
	const int GL_ShaderType[] =
	{
		-1, // Error (Used for placeholder)

		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER,
		GL_TESS_CONTROL_SHADER,
		GL_TESS_EVALUATION_SHADER,
		GL_COMPUTE_SHADER
	};
	const char* GL_ShaderNames[] =
	{
		"NULL", // Error (Used for placeholder)

		"VERTEX_SHADER",
		"FRAGMENT_SHADER",
		"GEOMETRY_SHADER",
		"TESS_CONTROL_SHADER",
		"TESS_EVALUATION_SHADER",
		"COMPUTE_SHADER"
	};
	const GLbitfield GL_BufferBit[] =
	{
		0, // Error (Used for placeholder) [0 used to prevent blit error]

		GL_COLOR_BUFFER_BIT,
		GL_DEPTH_BUFFER_BIT,
		GL_STENCIL_BUFFER_BIT,
		GL_ACCUM_BUFFER_BIT
	};
	const int GL_BlendSource[] =
	{
		-1, // Error (Used for placeholder)

		GL_ZERO,
		GL_ONE,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	};
	const int GL_BlendDestination[] =
	{
		-1, // Error (Used for placeholder)

		GL_ZERO,
		GL_ONE,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	};
	const int GL_BlendEquation[] =
	{
		-1, // Error (Used for placeholder)

		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_MIN,
		GL_MAX
	};
	const int GL_DepthPassRule[] =
	{
		-1, // Error (Used for placeholder)

		GL_LESS,
		GL_EQUAL,
		GL_GREATER,
		GL_LEQUAL,
		GL_GEQUAL,
		GL_NOTEQUAL,
		GL_ALWAYS
	};
	const int GL_DataType[] =
	{
		-1, // Error (Used for placeholder)

		GL_BYTE,
		GL_UNSIGNED_BYTE,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_HALF_FLOAT,
		GL_FLOAT,
		GL_DOUBLE,
		GL_FIXED,
		GL_INT_2_10_10_10_REV,
		GL_UNSIGNED_INT_2_10_10_10_REV,
		GL_UNSIGNED_INT_10F_11F_11F_REV
	};
	const int GL_BufferUsage[] =
	{
		-1, // Error (Used for placeholder)

		GL_STREAM_DRAW, // The data store contents will be modified once and used at most a few times.
		GL_STREAM_READ,
		GL_STREAM_COPY,
		GL_STATIC_DRAW, // The data store contents will be modified once and used many times.
		GL_STATIC_READ,
		GL_STATIC_COPY,
		GL_DYNAMIC_DRAW, // The data store contents will be modified repeatedly and used many times.
		GL_DYNAMIC_READ,
		GL_DYNAMIC_COPY
	};
	const int GL_DrawType[] =
	{
		-1, // Error (Used for placeholder)

		GL_POINTS,						// Geom / Frag	(points)
		GL_LINES,						// Geom / Frag	(lines)
		GL_LINE_STRIP,					// Geom / Frag	(lines)
		GL_LINE_LOOP,					// Geom / Frag	(lines)
		GL_LINES_ADJACENCY,				// Geom			(lines adjacency)
		GL_LINE_STRIP_ADJACENCY,		// Geom			(lines adjacency)
		GL_TRIANGLES,					// Geom / Frag	(triangles)
		GL_TRIANGLE_STRIP,				// Geom / Frag	(triangles)
		GL_TRIANGLE_FAN,				// Geom / Frag	(triangles)
		GL_TRIANGLES_ADJACENCY,			// Geom			(triangles adjacency)
		GL_TRIANGLE_STRIP_ADJACENCY,	// Geom (triangles adjacency)
		GL_PATCHES						// Tesselation Control
	};
	const int GL_TextureType[] =
	{
		-1, // Error (Used for placeholder)

		GL_TEXTURE_1D,
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_1D_ARRAY,
		GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_RECTANGLE,
		GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_CUBE_MAP_ARRAY,
		GL_TEXTURE_BUFFER,
		GL_TEXTURE_2D_MULTISAMPLE,
		GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
		GL_RENDERBUFFER
	};
	const int GL_TextureWrapping[] =
	{
		-1, // Error (Used for placeholder)

		GL_REPEAT,
		GL_CLAMP_TO_BORDER,
		GL_CLAMP_TO_EDGE,
		GL_MIRRORED_REPEAT,
		GL_MIRROR_CLAMP_TO_EDGE
	};
	const int GL_TextureFiltering[] =
	{
		-1, // Error (Used for placeholder)

		GL_NEAREST,
		GL_LINEAR
	};
	const int GL_TextureFormat[] =
	{
		-1, // Error (Used for placeholder)

		// 1 channel
		GL_R8,
		GL_R8_SNORM,
		GL_R16,
		GL_R16_SNORM,
		GL_R16F,
		// 2 channels
		GL_RG8,
		GL_RG8_SNORM,
		GL_RG16,
		GL_RG16_SNORM,
		GL_RG16F,
		// 3 channels
		GL_RGB8,
		GL_RGB8_SNORM,
		GL_RGB16,
		GL_RGB16_SNORM,
		GL_RGB16F,
		// 4 channels
		GL_RGBA8,
		GL_RGBA8_SNORM,
		GL_RGBA16,
		GL_RGBA16_SNORM,
		GL_RGBA16F,
		GL_R11F_G11F_B10F,// GOOD FOR GBUFFER

		// [ Special ]
		GL_SRGB8,
		GL_SRGB8_ALPHA8,

		// [DepthTextureFormat]
		GL_STENCIL_INDEX8,
		GL_DEPTH_COMPONENT16,
		GL_DEPTH_COMPONENT24,
		GL_DEPTH_COMPONENT32,
		GL_DEPTH_COMPONENT32F,
		GL_DEPTH24_STENCIL8,
		GL_DEPTH32F_STENCIL8
	};
	const int GL_TextureDepthFormat[] =
	{
		-1, // Error (Used for placeholder)

		GL_STENCIL_INDEX8,
		GL_DEPTH_COMPONENT16,
		GL_DEPTH_COMPONENT24,
		GL_DEPTH_COMPONENT32,
		GL_DEPTH_COMPONENT32F,
		GL_DEPTH24_STENCIL8,
		GL_DEPTH32F_STENCIL8
	};
	const int GL_TextureChannelType[] =
	{
		-1, // Error (Used for placeholder)

		GL_RED,	// 1 channel
		GL_RG,	// 2 channels
		GL_RGB,	// 3 channels
		GL_RGBA,// 4 channels

		GL_BGR,	// 3 channels
		GL_BGRA, // 4 channels

		GL_STENCIL_INDEX, // ? channels 
		GL_DEPTH_COMPONENT, // ? channels
		GL_DEPTH_STENCIL, // ? channels
	};
	const int GL_TexturePixelType[] =
	{
		-1, // Error (Used for placeholder)

		GL_UNSIGNED_BYTE,
		GL_BYTE,
		GL_UNSIGNED_SHORT,
		GL_SHORT,
		GL_UNSIGNED_INT,
		GL_INT,
		GL_HALF_FLOAT,
		GL_FLOAT,
		GL_UNSIGNED_BYTE_3_3_2,
		GL_UNSIGNED_SHORT_5_6_5,
		GL_UNSIGNED_SHORT_4_4_4_4,
		GL_UNSIGNED_SHORT_5_5_5_1,
		GL_UNSIGNED_INT_8_8_8_8,
		GL_UNSIGNED_INT_10_10_10_2
	};
	const int GL_TextureLayer[] =
	{
		-1, // Error (Used for placeholder)

		GL_TEXTURE0,
		GL_TEXTURE1,
		GL_TEXTURE2,
		GL_TEXTURE3,
		GL_TEXTURE4,
		GL_TEXTURE5,
		GL_TEXTURE6,
		GL_TEXTURE7,
		GL_TEXTURE8,
	};
	const GLenum GL_ColorAttachments[] =
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
	const int GL_QueryType[] =
	{
		GL_SAMPLES_PASSED,
		GL_ANY_SAMPLES_PASSED,
		GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
		GL_PRIMITIVES_GENERATED,
		GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
		GL_TIME_ELAPSED
	};
	const int GL_CubemapFace[] =
	{
		-1,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	// Error Message
	void CALLBACK OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data)
	{
		char buffer[9] = { '\0' };
		sprintf_s(buffer, "%.8x", id);

		std::string message("OpenGL(0x");
		message += buffer;
		message += "): ";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			message += "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			message += "Deprecated behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			message += "Undefined behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			message += "Portability issue";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			message += "Performance issue";
			break;
		case GL_DEBUG_TYPE_MARKER:
			message += "Stream annotation";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
			message += "Other";
			break;
		}

		message += " \nSource: ";
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			message += "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			message += "Window system";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			message += "Shader compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			message += "Third party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			message += "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			message += "Other";
		}

		message += " \nSeverity: ";
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			message += "HIGH";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			message += "Medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			message += "Low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			message += "Notification";
		}

		message += " \n";
		message += msg;
		message += " \n";

		if (type == GL_DEBUG_TYPE_ERROR || severity == GL_DEBUG_SEVERITY_HIGH)
		{
			Logger.error(message);
			//std::system("PAUSE");
		}
		else
		{
			// For some reason it hates how im storing my VBOs/EBOs
			//Logger.log(message);
		}
	}
	void Graphics::InitOpenGLDebugCallback(void)
	{
		// Debug CallBack
#if defined(GLOBAL_ERROR_CALLBACK) && !defined(GLOBAL_USE_GLNAMES)
		//GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		//if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			UsingErrorCallback = true;
			glDebugMessageCallback(OpenGLDebugCallback, stderr);
			//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
		}
#endif
	}

	// ~ Setup ~ //
	bool Graphics::Setup()
	{
		// Init Glew
		if (gl3wInit())
		{
			Logger.error("failed to initialize OpenGL");
			std::system("pause");
			return false;
		}

		// Acquire Version
		glGetIntegerv(GL_MAJOR_VERSION, &GLVersionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &GLVersionMinor);

		// Check GL Version
		if (!gl3wIsSupported(GLVersionMajor, GLVersionMinor))
		{
			Logger.error("OpenGL " + std::to_string(GLVersionMajor) + '.' + std::to_string(GLVersionMinor) + " not supported");
			std::system("pause");
			return false;
		}

		// Acquire GPU info
		Gpu_Renderer = std::string((reinterpret_cast<char const*>(glGetString(GL_RENDERER))));
		Gpu_OpenGLVersion = std::string((reinterpret_cast<char const*>(glGetString(GL_VERSION))));
		Gpu_Vendor = std::string((reinterpret_cast<char const*>(glGetString(GL_VENDOR))));

		// Acquire Vendor
		std::string VendorStr = Gpu_Vendor;
		std::transform(VendorStr.begin(), VendorStr.end(), VendorStr.begin(), ::tolower);
		if (VendorStr.find("nvidia") != std::string::npos)
			Vendor = VendorType::NVIDIA;
		else if (VendorStr.find("ati") != std::string::npos || VendorStr.find("radeon") != std::string::npos)
			Vendor = VendorType::AMD;
		else if (VendorStr.find("intel") != std::string::npos)
			Vendor = VendorType::INTEL;

		// Acquire Restrictions
		glGetIntegerv(GL_MAX_LABEL_LENGTH, &GLObjectNameMaxLength);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &GLMaxAnisotropy);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &GLMaxFBOColorAttachments);
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &GLMaxUniformBindings);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &GLMaxAttributes);

		// VRAM Maximum
		if (Vendor == VendorType::NVIDIA)
		{
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &VRAM_Maximum_KB);
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &VRAM_Current_KB);
		}

		// All good
		return true;
	}
	void Graphics::initHints()
	{
		// Reset cpp variables
		gl_cullmode = CullMode::NONE;
		gl_blendState = false;
		gl_blendsrc = BlendSource::NONE;
		gl_blenddest = BlendDestination::NONE;
		gl_blendequation = BlendEquation::NONE;
		gl_depthpassrule = DepthPassRule::NONE;
		gl_depthWrite = false;
		gl_depthRead = false;
		gl_wireframe = false;
		gl_seamlessCubemaps = false;
		for (int i = 0; i < 4; i++)
			gl_viewport[i] = -1;
		gl_lineWidth = 1.0f;

		gl_activeTextureId = 0;
		gl_activeTextureLayer = TextureLevel::NONE;

		gl_activeBufferId = 0;

		gl_activeFBO = 0;
		gl_activeShaderProgram = 0;

		gl_activeVAO = 0;

		gl_pixelPackAlignment = PixelAlignment::NONE;
		gl_pixelUnpackAlignment = PixelAlignment::NONE;

		// Set Default Cull Mode
		SetCullMode(CullMode::COUNTER_CLOCKWISE);
		// Set Default Blend info
		SetBlendState(true);
		SetBlendMode(BlendSource::SRC_ALPHA, BlendDestination::ONE_MINUS_SRC_ALPHA);
		SetBlendEquation(BlendEquation::FUNC_ADD);
		// Set Default Depth info
		SetDepthRead(true);
		SetDepthPassRule(DepthPassRule::LESS_OR_EQUAL);
		// Set Default Wireframe info
		SetWireframeState(false);
		// Cubemap edges are hidden
		SetSeamlessCubemap(true);
		// Pixel pack/unpack
		Graphics::Texture::SetPixelPackAlignment(PixelAlignment::ALIGN_4);
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_4);

		// Enable Debug Outputs // Important for queries
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	// Updates specific values
	void Graphics::GetRuntimeGLValues(void)
	{
		if (Vendor == VendorType::NVIDIA)
		{
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &VRAM_Current_KB);
		}

		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

		CPU::VirtualMemory::Total_KB = memInfo.ullTotalPageFile / 1024;
		CPU::VirtualMemory::Used_KB = (memInfo.ullTotalPageFile - memInfo.ullAvailPageFile) / 1024;
		CPU::VirtualMemory::UsedByProcess_KB = pmc.PrivateUsage / 1024;

		CPU::RAM::Total_KB = memInfo.ullTotalPhys / 1024;
		CPU::RAM::Used_KB = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024;
		CPU::RAM::UsedByProcess_KB = pmc.WorkingSetSize / 1024;
	}

	// Check if string name matches GL type
	bool Graphics::VeryifyDataType(const std::string& name, uint32_t type)
	{
		// Bool
		if (name.compare("bool") == 0 && type == GL_BOOL)
			return true;
		// Int
		if (name.compare("int") == 0 && type == GL_INT)
			return true;
		// Unsigned Int
		if (name.compare("unsigned int") == 0 && type == GL_UNSIGNED_INT)
			return true;
		// Float
		if (name.compare("float") == 0 && type == GL_FLOAT)
			return true;
		// Double
		if (name.compare("double") == 0 && type == GL_DOUBLE)
			return true;
		// Vec2
		if (name.compare("class Vxl::_Vector2<float>") == 0 && type == GL_FLOAT_VEC2)
			return true;
		// Vec3 - Color3F
		if ((name.compare("class Vxl::_Vector3<float>") == 0 || name.compare("class Vxl::Color3F") == 0) && type == GL_FLOAT_VEC3)
			return true;
		// Vec4 - Color4F
		if ((name.compare("class Vxl::_Vector4<float>") == 0 || name.compare("class Vxl::Color4F") == 0) && type == GL_FLOAT_VEC4)
			return true;
		// Mat2
		if (name.compare("class Vxl::Matrix2x2") == 0 && type == GL_FLOAT_MAT2)
			return true;
		// Mat3
		if (name.compare("class Vxl::Matrix3x3") == 0 && type == GL_FLOAT_MAT3)
			return true;
		// Mat4
		if (name.compare("class Vxl::Matrix4x4") == 0 && type == GL_FLOAT_MAT4)
			return true;

		return false;
	}

	
	// ~ GPU Name ~ //
	void Graphics::SetGLName(ObjectType identifier, uint32_t id, const std::string &label)
	{
#if defined(GLOBAL_USE_GLNAMES) && !defined(GLOBAL_ERROR_CALLBACK)
		//
		std::string labelEdit = label + " (" + std::to_string(id) + ")";
		VXL_ASSERT(labelEdit.size() < GLObjectNameMaxLength, "GLName is too big");
		glObjectLabel(GL_ObjectType[(int)identifier], id, static_cast<GLsizei>(labelEdit.size()), labelEdit.c_str());
		//
#endif
	}

	// ~ States ~ //
	void Graphics::SetCullMode(CullMode cullmode)
	{
		if (gl_cullmode != cullmode)
		{
			switch (cullmode)
			{
			case CullMode::NO_CULL:
				glDisable(GL_CULL_FACE);
				break;

			case CullMode::COUNTER_CLOCKWISE:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				break;

			case CullMode::CLOCKWISE:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CW);
				break;
			default:
				VXL_ASSERT(FALSE, "GL ERROR: Invalid CullMode");
				break;
			}
		}

		gl_cullmode = cullmode;
	}
	void Graphics::SetBlendState(bool state)
	{
		if (gl_blendState == state)
			return;

		if (state)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		gl_blendState = state;
	}
	void Graphics::SetDepthRead(bool state)
	{
		if (gl_depthRead == state)
			return;

		if (state)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		gl_depthRead = state;
	}
	void Graphics::SetWireframeState(bool state)
	{
		if (gl_wireframe == state)
			return;
		
		if (state)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		gl_wireframe = state;
	}
	void Graphics::SetDepthWrite(bool state)
	{
		if (gl_depthWrite == state)
			return;

		glDepthMask(state);
		gl_depthWrite = state;
	}
	void Graphics::SetSeamlessCubemap(bool state)
	{
		if (gl_seamlessCubemaps == state)
			return;

		if (state)
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		else
			glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		gl_seamlessCubemaps = state;
	}

	// ~ State Getters ~ //
	bool Graphics::GetDepthRead()
	{
		return gl_depthRead;
	}

	// ~ Debug ~ //
	bool Graphics::Debug::GetDepthRead()
	{
		GLboolean value;
		glGetBooleanv(GL_DEPTH_TEST, &value);
		return value;
	}

	// ~ Blending ~ //
	void Graphics::SetBlendMode(BlendSource src, BlendDestination dest)
	{
		if (gl_blendsrc == src && gl_blenddest == dest)
			return;

		glBlendFunc(GL_BlendSource[(int)src], GL_BlendDestination[(int)dest]);
		gl_blendsrc = src;
		gl_blenddest = dest;
	}
	void Graphics::SetBlendEquation(BlendEquation eq)
	{
		if (gl_blendequation == eq)
			return;

		glBlendEquation(GL_BlendEquation[(int)eq]);
		gl_blendequation = eq;
	}
	void Graphics::SetBlendMode(uint32_t attachmentIndex, BlendSource src, BlendDestination dest)
	{
		glBlendFunci(attachmentIndex, GL_BlendSource[(int)src], GL_BlendDestination[(int)dest]);
	}

	// ~ Depth ~ //
	void Graphics::SetDepthPassRule(DepthPassRule rule)
	{
		if (gl_depthpassrule == rule)
			return;

		glDepthFunc(GL_DepthPassRule[(int)rule]);
		gl_depthpassrule = rule;
	}

	// ~ Viewport ~ //
	void Graphics::SetViewport(int x, int y, int w, int h)
	{
		if (gl_viewport[0] == x && gl_viewport[1] == y && gl_viewport[2] == w && gl_viewport[3] == h)
			return;
		
		glViewport(x, y, w, h);	
		gl_viewport[0] = x;
		gl_viewport[1] = y;
		gl_viewport[2] = w;
		gl_viewport[3] = h;
	}

	// ~ LineWidth ~ //
	void Graphics::SetLineWidth(float w)
	{
		// No chnage to float
		if (fabs(gl_lineWidth - w) < FLT_EPSILON)
			return;

		glLineWidth(w);
		gl_lineWidth = w;
	}

	// ~ Clear Buffer [Setup] ~ //
	void Graphics::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	void Graphics::SetClearColor(const Color3F& c, float a)
	{
		glClearColor(c.r, c.g, c.b, a);
	}
	void Graphics::SetClearColor(const Color4F& c)
	{
		glClearColor(c.r, c.g, c.b, c.a);
	}
	void Graphics::SetClearDepth(float f)
	{
		VXL_ASSERT(f >= 0.0f && f <= 1.0f, "Depth Value must be between 0 and 1 inclusif");
		glClearDepth(f);
	}
	void Graphics::SetClearStencil(int i)
	{
		glClearStencil(i);
	}
	// ~ Clear Buffer [Call] ~ //
	void Graphics::ClearBuffer(BufferBit clearBit)
	{
		glClear(GL_BufferBit[(int)clearBit]);
	}
	void Graphics::ClearBuffer(BufferBit clearBit, BufferBit clearBit2)
	{
		glClear(GL_BufferBit[(int)clearBit] | GL_BufferBit[(int)clearBit2]);
	}
	void Graphics::ClearAllBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	void Graphics::ClearBufferFBOAttachment(uint32_t attachmentIndex, float r, float g, float b, float a)
	{
		float c[4] = { r, g, b, a };
		glClearBufferfv(GL_COLOR, attachmentIndex, c);
	}
	void Graphics::ClearBufferFBOAttachment(uint32_t attachmentIndex, const Color3F& clearColor, float a)
	{
		float c[4] = { clearColor.r, clearColor.g, clearColor.b, a };
		glClearBufferfv(GL_COLOR, attachmentIndex, c);
	}
	void Graphics::ClearBufferFBOAttachment(uint32_t attachmentIndex, const Color4F& clearColor)
	{
		float c[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
		glClearBufferfv(GL_COLOR, attachmentIndex, c);
	}

	// ~ Conversion ~ //
	uint32_t Graphics::GetValueCount(AttributeType type)
	{
		switch (type)
		{
		case AttributeType::INT:
		case AttributeType::UINT:
		case AttributeType::FLOAT:
		case AttributeType::DOUBLE:
			return 1;
		case AttributeType::VEC2:
			return 2;
		case AttributeType::VEC3:
			return 3;
		case AttributeType::VEC4:
		case AttributeType::MAT2:
			return 4;
		case AttributeType::MAT3:
			return 9;
		case AttributeType::MAT4:
			return 16;
		default:
			VXL_ASSERT(false, "Invalid AttributeType");
			return 0;
		}
	}
	uint32_t Graphics::GetSize(AttributeType type)
	{
		switch (type)
		{
		case AttributeType::INT:
		case AttributeType::UINT:
		case AttributeType::FLOAT:
			return 4;
		case AttributeType::DOUBLE:
		case AttributeType::VEC2:
			return 8;
		case AttributeType::VEC3:
			return 12;
		case AttributeType::VEC4:
		case AttributeType::MAT2:
			return 16;
		case AttributeType::MAT3:
			return 36;
		case AttributeType::MAT4:
			return 64;
		default:
			VXL_ASSERT(false, "Invalid AttributeType");
			return 0;
		}
	}
	DataType Graphics::GetDataType(AttributeType type)
	{
		switch (type)
		{
		case AttributeType::INT:
			return DataType::INT;

		case AttributeType::UINT:
			return DataType::UNSIGNED_INT;

		case AttributeType::FLOAT:
		case AttributeType::VEC2:
		case AttributeType::VEC3:
		case AttributeType::VEC4:
		case AttributeType::MAT2:
		case AttributeType::MAT3:
		case AttributeType::MAT4:
			return DataType::FLOAT;

		case AttributeType::DOUBLE:
			return DataType::DOUBLE;

		default:
			VXL_ASSERT(false, "Invalid ShaderDataType");
			return DataType::NONE;
		}
	}
	DrawSubType Graphics::GetDrawSubType(DrawType type)
	{
		switch (type)
		{
		case DrawType::TRIANGLES:
		case DrawType::TRIANGLES_ADJACENCY:
		case DrawType::TRIANGLE_FAN:
		case DrawType::TRIANGLE_STRIP:
		case DrawType::TRIANGLE_STRIP_ADJACENCY:
			return DrawSubType::TRIANGLES;

		case DrawType::LINES:
		case DrawType::LINES_ADJACENCY:
		case DrawType::LINE_LOOP:
		case DrawType::LINE_STRIP:
		case DrawType::LINE_STRIP_ADJACENCY:
			return DrawSubType::LINES;

		case DrawType::POINTS:
			return DrawSubType::POINTS;

		default:
			VXL_ASSERT(false, "Unknown DrawType for GetDrawSubType()");
			return DrawSubType::TRIANGLES;
		}
	}
	TexturePixelType Graphics::GetPixelData(TextureDepthFormat format)
	{
		switch (format)
		{
		case TextureDepthFormat::STENCIL8:
			return TexturePixelType::UNSIGNED_BYTE;
		
		case TextureDepthFormat::DEPTH16:
			return TexturePixelType::UNSIGNED_SHORT;
		
		case TextureDepthFormat::DEPTH24:
			return TexturePixelType::UNSIGNED_INT_8_8_8_8;
		
		case TextureDepthFormat::DEPTH24_STENCIL8:
			return TexturePixelType::UNSIGNED_INT_8_8_8_8;
		
		case TextureDepthFormat::DEPTH32:
		case TextureDepthFormat::DEPTH32F:
			return TexturePixelType::UNSIGNED_INT;
		
		case TextureDepthFormat::DEPTH32F_STENCIL8:
			return TexturePixelType::UNSIGNED_INT;

		default:
			VXL_ASSERT(false, "Invalid DepthFormat Type");
			return TexturePixelType::NONE;
		}
	}
	TextureChannelType Graphics::GetChannelType(int ChannelCount)
	{
		switch (ChannelCount)
		{
		default:
		case 0:
			return TextureChannelType::NONE;
		case 1:
			return TextureChannelType::R;
		case 2:
			return TextureChannelType::RG;
		case 3:
			return TextureChannelType::RGB;
		case 4:
			return TextureChannelType::RGBA;
		}

		VXL_ASSERT(false, "Invalid Channel Count");
		return TextureChannelType::NONE;
	}
	TextureChannelType Graphics::GetChannelType(TextureFormat format)
	{
		switch (format)
		{
			// 1 Channel
		case TextureFormat::R8:
		case TextureFormat::R8_SNORM:
		case TextureFormat::R16:
		case TextureFormat::R16_SNORM:
		case TextureFormat::R16F:
			return TextureChannelType::R;
			
			// 2 Channels
		case TextureFormat::RG8:
		case TextureFormat::RG8_SNORM:
		case TextureFormat::RG16:
		case TextureFormat::RG16_SNORM:
		case TextureFormat::RG16F:
			return TextureChannelType::RG;
			
			// 3 Channels
		case TextureFormat::RGB8:
		case TextureFormat::RGB8_SNORM:
		case TextureFormat::RGB16:
		case TextureFormat::RGB16_SNORM:
		case TextureFormat::RGB16F:
		case TextureFormat::R11F_G11F_B10F:// Special
		case TextureFormat::SRGB8:// Special
			return TextureChannelType::RGB;
			
			// 4 ChannelS
		case TextureFormat::RGBA8:
		case TextureFormat::RGBA8_SNORM:
		case TextureFormat::RGBA16:
		case TextureFormat::RGBA16_SNORM:
		case TextureFormat::RGBA16F:
		case TextureFormat::SRGBA8:// Special
			return TextureChannelType::RGBA;

			// Depth
		case TextureFormat::DEPTH16:// Special
		case TextureFormat::DEPTH24:// Special
		case TextureFormat::DEPTH32:// Special
		case TextureFormat::DEPTH32F:// Special
			return TextureChannelType::DEPTH;
			
			// Stencil
		case TextureFormat::STENCIL8:// Special
			return TextureChannelType::STENCIL;
			
			// Stencil + Depth
		case TextureFormat::DEPTH24_STENCIL8:// Special
		case TextureFormat::DEPTH32F_STENCIL8:// Special
			return TextureChannelType::DEPTH_STENCIL;
		}

		VXL_ASSERT(false, "Invalid Texture Format");
		return TextureChannelType::NONE;
	}
	uint32_t Graphics::GetChannelCount(TextureChannelType type)
	{
		// Solve channel count based on channel type
		switch (type)
		{
		case TextureChannelType::NONE:
			return 0;
		case TextureChannelType::R:
			return 1;
		case TextureChannelType::RG:
			return 2;
		case TextureChannelType::RGB:
		case TextureChannelType::BGR:
			return 3;
		case TextureChannelType::RGBA:
		case TextureChannelType::BGRA:
			return 4;
			// Special Case [Cannot figure out channel count based on this information alone]
		case TextureChannelType::DEPTH:
		case TextureChannelType::DEPTH_STENCIL:
			return -1;
		}

		VXL_ASSERT(false, "Invalid Channel Type");
		return 0;
	}
	uint32_t Graphics::GetChannelCount(TextureFormat format)
	{
		// Special exceptions
		switch (format)
		{
			// 1 Channel
		case TextureFormat::R8:
		case TextureFormat::R8_SNORM:
		case TextureFormat::R16:
		case TextureFormat::R16_SNORM:
		case TextureFormat::R16F:
		case TextureFormat::STENCIL8:
			return 1;
			// 2 Channels
		case TextureFormat::RG8:
		case TextureFormat::RG8_SNORM:
		case TextureFormat::RG16:
		case TextureFormat::RG16_SNORM:
		case TextureFormat::RG16F:
		case TextureFormat::DEPTH16:// Special
			return 2;
			// 3 Channels
		case TextureFormat::RGB8:
		case TextureFormat::RGB8_SNORM:
		case TextureFormat::RGB16:
		case TextureFormat::RGB16_SNORM:
		case TextureFormat::RGB16F:
		case TextureFormat::R11F_G11F_B10F:// Special
		case TextureFormat::SRGB8:// Special
		case TextureFormat::DEPTH24:// Special
			return 3;
			// 4 Channels
		case TextureFormat::RGBA8:
		case TextureFormat::RGBA8_SNORM:
		case TextureFormat::RGBA16:
		case TextureFormat::RGBA16_SNORM:
		case TextureFormat::RGBA16F:
		case TextureFormat::SRGBA8:// Special
		case TextureFormat::DEPTH32:// Special
		case TextureFormat::DEPTH32F:// Special
		case TextureFormat::DEPTH24_STENCIL8:// Special
			return 4;
		case TextureFormat::DEPTH32F_STENCIL8:// Special
			return 8; // 64 bits total (24 bits padding for alignment)
		}

		VXL_ASSERT(false, "Invalid Format Type");
		return 0;
	}
	TextureFormat Graphics::GetFormat(TextureDepthFormat format)
	{
		switch (format)
		{
		case TextureDepthFormat::STENCIL8:
			return TextureFormat::STENCIL8;

		case TextureDepthFormat::DEPTH16:
			return TextureFormat::DEPTH16;

		case TextureDepthFormat::DEPTH24:
			return TextureFormat::DEPTH24;

		case TextureDepthFormat::DEPTH32:
			return TextureFormat::DEPTH32;

		case TextureDepthFormat::DEPTH32F:
			return TextureFormat::DEPTH32F;

		case TextureDepthFormat::DEPTH24_STENCIL8:
			return TextureFormat::DEPTH24_STENCIL8;

		case TextureDepthFormat::DEPTH32F_STENCIL8:
			return TextureFormat::DEPTH32F_STENCIL8;

		default:
			VXL_ASSERT(false, "TextureDepthFormat value not recognized for GetFormat()");
			return TextureFormat::DEPTH16;
		}
	}

	// Copy Texture
	void Graphics::CopyTexture(
		TextureID src, TextureType srcType,
		TextureID dest, TextureType destType,
		uint32_t width, uint32_t height
	)
	{
		glCopyImageSubData(
			src, GL_TextureType[(int)srcType], 0, 0, 0, 0,
			dest, GL_TextureType[(int)destType], 0, 0, 0, 0,
			width, height, 1
		);
	}

	// ~ Sends Uniforms to Shader ~ //
	template<>
	void Graphics::Uniform::Send<bool>(bool data)
	{
		glUniform1i(location, (int)data);
	}
	template<>
	void Graphics::Uniform::Send<int>(int data)
	{
		glUniform1i(location, data);
	}
	template<>
	void Graphics::Uniform::Send<uint32_t>(uint32_t data)
	{
		glUniform1ui(location, data);
	}
	template<>
	void Graphics::Uniform::Send<float>(float data)
	{
		glUniform1f(location, data);
	}
	template<>
	void Graphics::Uniform::Send<double>(double data)
	{
		glUniform1d(location, data);
	}
	template<>
	void Graphics::Uniform::Send<Vector2>(Vector2 data)
	{
		glUniform2f(location, data.x, data.y);
	}
	template<>
	void Graphics::Uniform::Send<Vector2&>(Vector2& data)
	{
		glUniform2f(location, data.x, data.y);
	}
	template<>
	void Graphics::Uniform::Send<Vector3>(Vector3 data)
	{
		glUniform3f(location, data.x, data.y, data.z);
	}
	template<>
	void Graphics::Uniform::Send<Vector3&>(Vector3& data)
	{
		glUniform3f(location, data.x, data.y, data.z);
	}
	template<>
	void Graphics::Uniform::Send<Vector4>(Vector4 data)
	{
		glUniform4f(location, data.x, data.y, data.z, data.w);
	}
	template<>
	void Graphics::Uniform::Send<Vector4&>(Vector4& data)
	{
		glUniform4f(location, data.x, data.y, data.z, data.w);
	}
	template<>
	void Graphics::Uniform::Send<Color3F>(Color3F data)
	{
		glUniform3f(location, data.r, data.g, data.b);
	}
	template<>
	void Graphics::Uniform::Send<Color3F&>(Color3F& data)
	{
		glUniform3f(location, data.r, data.g, data.b);
	}
	template<>
	void Graphics::Uniform::Send<Color4F>(Color4F data)
	{
		glUniform4f(location, data.r, data.g, data.b, data.a);
	}
	template<>
	void Graphics::Uniform::Send<Color4F&>(Color4F& data)
	{
		glUniform4f(location, data.r, data.g, data.b, data.a);
	}

	template<>
	void Graphics::Uniform::SendMatrix<Matrix2x2>(Matrix2x2 data, bool transposeMatrix)
	{
		glUniformMatrix2fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix2x2&>(Matrix2x2& data, bool transposeMatrix)
	{
		glUniformMatrix2fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3>(Matrix3x3 data, bool transposeMatrix)
	{
		glUniformMatrix3fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3&>(Matrix3x3& data, bool transposeMatrix)
	{
		glUniformMatrix3fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix4x4>(Matrix4x4 data, bool transposeMatrix)
	{
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix4x4&>(Matrix4x4& data, bool transposeMatrix)
	{
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
	}

	template<>
	void Graphics::Uniform::Send<bool>(ShaderProgramID id, bool data)
	{
		glProgramUniform1i(id, location, (int)data);
	}
	template<>
	void Graphics::Uniform::Send<int>(ShaderProgramID id, int data)
	{
		glProgramUniform1i(id, location, data);
	}
	template<>
	void Graphics::Uniform::Send<uint32_t>(ShaderProgramID id, uint32_t data)
	{
		glProgramUniform1ui(id, location, data);
	}
	template<>
	void Graphics::Uniform::Send<float>(ShaderProgramID id, float data)
	{
		glProgramUniform1f(id, location, data);
	}
	template<>
	void Graphics::Uniform::Send<double>(ShaderProgramID id, double data)
	{
		glProgramUniform1d(id, location, data);
	}
	template<>
	void Graphics::Uniform::Send<Vector2>(ShaderProgramID id, Vector2 data)
	{
		glProgramUniform2f(id, location, data.x, data.y);
	}
	template<>
	void Graphics::Uniform::Send<Vector2&>(ShaderProgramID id, Vector2& data)
	{
		glProgramUniform2f(id, location, data.x, data.y);
	}
	template<>
	void Graphics::Uniform::Send<Vector3>(ShaderProgramID id, Vector3 data)
	{
		glProgramUniform3f(id, location, data.x, data.y, data.z);
	}
	template<>
	void Graphics::Uniform::Send<Vector3&>(ShaderProgramID id, Vector3& data)
	{
		glProgramUniform3f(id, location, data.x, data.y, data.z);
	}
	template<>
	void Graphics::Uniform::Send<Vector4>(ShaderProgramID id, Vector4 data)
	{
		glProgramUniform4f(id, location, data.x, data.y, data.z, data.w);
	}
	template<>
	void Graphics::Uniform::Send<Vector4&>(ShaderProgramID id, Vector4& data)
	{
		glProgramUniform4f(id, location, data.x, data.y, data.z, data.w);
	}
	template<>
	void Graphics::Uniform::Send<Color3F>(ShaderProgramID id, Color3F data)
	{
		glProgramUniform3f(id, location, data.r, data.g, data.b);
	}
	template<>
	void Graphics::Uniform::Send<Color3F&>(ShaderProgramID id, Color3F& data)
	{
		glProgramUniform3f(id, location, data.r, data.g, data.b);
	}
	template<>
	void Graphics::Uniform::Send<Color4F>(ShaderProgramID id, Color4F data)
	{
		glProgramUniform4f(id, location, data.r, data.g, data.b, data.a);
	}
	template<>
	void Graphics::Uniform::Send<Color4F&>(ShaderProgramID id, Color4F& data)
	{
		glProgramUniform4f(id, location, data.r, data.g, data.b, data.a);
	}

	template<>
	void Graphics::Uniform::SendMatrix<Matrix2x2>(ShaderProgramID id, Matrix2x2 data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix2x2&>(ShaderProgramID id, Matrix2x2& data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3>(ShaderProgramID id, Matrix3x3 data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3&>(ShaderProgramID id, Matrix3x3& data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix4x4>(ShaderProgramID id, Matrix4x4 data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix4x4&>(ShaderProgramID id, Matrix4x4& data, bool transposeMatrix)
	{
		glProgramUniformMatrix4fv(id, location, 1, transposeMatrix, data.GetStartPointer());
	}

	// ~ Subroutine Functions ~ //
	void Graphics::UniformSubroutine::Connect(const std::string& UniformName, const std::string& FunctionName)
	{
		VXL_ASSERT(uniforms.find(UniformName) != uniforms.end(), "UniformSubroutine: Cannot find UniformName");
		VXL_ASSERT(functions.find(FunctionName) != functions.end(), "UniformSubroutine: Cannot find FunctionName");

		indices[uniforms[UniformName]] = functions[FunctionName];
	}
	void Graphics::UniformSubroutine::Bind() const
	{
		glUniformSubroutinesuiv(shaderType, indexCount, &indices[0]);
	}


	// ~ Shader ~ //
	ShaderID Graphics::Shader::Create(ShaderType shaderType)
	{
		ShaderID id = glCreateShader(GL_ShaderType[(int)shaderType]);

		VXL_ASSERT(id != -1, "GL ERROR: glCreateShader()");

		return id;
	}
	void Graphics::Shader::Delete(ShaderID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteShader()");

		glDeleteShader(id);
	}
	bool Graphics::Shader::Compile(ShaderID id, ShaderType shaderType, const char* source)
	{
		// Attach Source and Compile Shader
		glShaderSource(id, 1, &source, NULL);
		glCompileShader(id);

		// Check compilation status
		int compile_status;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);

		// Error
		if (compile_status == GL_FALSE)
		{
			Logger.error("GL ERROR: Failed To Compile " + std::string(GL_ShaderNames[(int)shaderType]));
		}

		// Status
		return (bool)compile_status;
	}
	std::string	Graphics::Shader::GetError(ShaderID id)
	{
		// Acquire error length
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		if (length <= 0)
		{
			Logger.error("Unable to retrieve Shader error log");
		}

		// Acquire message
		std::vector<char> error(length);
		glGetShaderInfoLog(id, length, &length, &error[0]);

		// End
		return std::string(error.begin(), error.end());
	}
	std::string Graphics::Shader::GetName(ShaderType shaderType)
	{
		return GL_ShaderNames[(int)shaderType];
	}

	// ~ Shader Program ~ //
	ShaderProgramID Graphics::ShaderProgram::Create(void)
	{
		ShaderProgramID program = glCreateProgram();

		VXL_ASSERT(program != -1, "GL ERROR: glCreateProgram()");

		return program;
	}
	void Graphics::ShaderProgram::Delete(ShaderProgramID program)
	{
		VXL_ASSERT(program != -1, "GL ERROR: glDeleteProgram()");

		glDeleteProgram(program);
	}
	void Graphics::ShaderProgram::AttachShader(ShaderProgramID program, ShaderID id)
	{
		glAttachShader(program, id);
	}
	void Graphics::ShaderProgram::DetachShader(ShaderProgramID program, ShaderID id)
	{
		glDetachShader(program, id);
	}
	bool Graphics::ShaderProgram::Link(ShaderProgramID program)
	{
		glLinkProgram(program);

		int status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (status == GL_FALSE)
		{
			Logger.error("GL ERROR: Failed To Link Shader Program");
		}

		return (bool)status;
	}
	bool Graphics::ShaderProgram::Validate(ShaderProgramID program)
	{
		glValidateProgram(program);

		int status = 0;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

		if (status == GL_FALSE)
		{
			Logger.error("GL ERROR: Failed To Validate Shader Program");
		}

		return (bool)status;
	}
	void Graphics::ShaderProgram::Enable(ShaderProgramID program)
	{
		if (gl_activeShaderProgram == program)
			return;

		glUseProgram(program);
		gl_activeShaderProgram = program;
	}
	void Graphics::ShaderProgram::Disable()
	{
		if (gl_activeShaderProgram == 0)
			return;

		glUseProgram(0);
		gl_activeShaderProgram = 0;
	}
	ShaderProgramID Graphics::ShaderProgram::GetCurrentlyActive(void)
	{
		return gl_activeShaderProgram;
	}
	std::string	Graphics::ShaderProgram::GetError(ShaderProgramID id)
	{
		// Acquire error length
		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		if (length <= 0)
		{
			return std::string("Unable to retrieve Shader Program error log");
		}
		else
		{
			// Acquire message
			std::vector<char> error(length);
			glGetProgramInfoLog(id, length, &length, &error[0]);

			// End
			return std::string(error.begin(), error.end());
		}
	}
	std::map<std::string, Graphics::Uniform> Graphics::ShaderProgram::AcquireUniforms(ShaderProgramID id)
	{
		const GLsizei bufSize = 128; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		GLint uniform_count;
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniform_count);
		//printf("Active Uniforms: %d\n", m_uniformCount);

		std::map<std::string, Graphics::Uniform> uniforms;
		
		// None found
		if (uniform_count == 0)
			return uniforms;
		
		for (GLint i = 0; i < uniform_count; i++)
		{
			Graphics::Uniform uniform;

			glGetActiveUniform(id, (GLuint)i, bufSize, &length, &uniform.size, &uniform.type, name);
			//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);

			uniform.location = glGetUniformLocation(id, name);
			uniform.name = std::string(name);

			uniforms[uniform.name] = uniform;
		}

		return uniforms;
	}
	std::map<std::string, Graphics::UniformBlock> Graphics::ShaderProgram::AcquireUniformBlocks(ShaderProgramID id)
	{
		GLint uniformBlockCount;
		glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);

		std::map<std::string, Graphics::UniformBlock> uniformBlocks;

		// None found
		if (uniformBlockCount == 0)
			return uniformBlocks;

		const GLsizei bufSize = 128; // maximum name length
		GLint nameLen;
		GLchar name[bufSize];

		for (int blockIx = 0; blockIx < uniformBlockCount; ++blockIx)
		{
			Graphics::UniformBlock uniformBlock;

			glGetActiveUniformBlockiv(id, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
			glGetActiveUniformBlockName(id, blockIx, nameLen, NULL, &name[0]);
			glGetActiveUniformBlockiv(id, blockIx, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlock.size);

			uniformBlock.blockIndex = glGetUniformBlockIndex(id, name);

			if (uniformBlock.blockIndex == -1)
			{
				VXL_ASSERT(false, "GL ERROR: BlockIndex not found");
			}

			// Get Binding point from name (ex: "ColorBlock_17" => Binding Point is 17)
			GLuint bindingPoint = -1;
			std::string bindingPointStr = "";
			uniformBlock.name = std::string(name);
			size_t loc = uniformBlock.name.find_last_of('_');
			if (loc != -1)
				bindingPointStr = uniformBlock.name.substr(loc + 1, nameLen);
			if (!bindingPointStr.empty())
				bindingPoint = std::stoi(bindingPointStr);

			if (bindingPoint == -1)
			{
				VXL_ASSERT(false, "Could not figure out binding point from name, ex: MyBuffer_1 == bindingPoint of 1");
				continue;
			}

			// Lock Block Index to Binding Point in shader
			glUniformBlockBinding(id, uniformBlock.blockIndex, bindingPoint);

			// Assign
			uniformBlocks[uniformBlock.name] = uniformBlock;
		}

		return uniformBlocks;
	}

	std::map<ShaderType, Graphics::UniformSubroutine> Graphics::ShaderProgram::AcquireUniformSubroutines(ShaderProgramID id, std::vector<Vxl::Shader*> shaders)
	{
		std::map<ShaderType, Graphics::UniformSubroutine> subroutines;

		char name[256]; int len, numCompS;

		uint32_t shaderCount = (uint32_t)shaders.size();
		for (uint32_t s = 0; s < shaderCount; s++)
		{
			int subroutineCount;
			GLenum type = GL_ShaderType[(int)shaders[s]->GetType()];

			glGetProgramStageiv(id, type, GL_ACTIVE_SUBROUTINE_UNIFORMS, &subroutineCount);

			if (subroutineCount > 0)
			{
				auto& Subroutine = subroutines[shaders[s]->GetType()];
				Subroutine = UniformSubroutine();
				Subroutine.shaderType = type;
				Subroutine.indices.resize(subroutineCount);
				Subroutine.indexCount = subroutineCount;

				for (int i = 0; i < subroutineCount; ++i)
				{
					glGetActiveSubroutineUniformName(id, type, i, 256, &len, name);

					//printf("Suroutine Uniform: %d name: %s\n", i, name);
					glGetActiveSubroutineUniformiv(id, type, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);

					//Subroutine.uniforms[name] = i;
					Subroutine.uniforms[name] = glGetSubroutineUniformLocation(id, type, name);

					int *s = (int *)malloc(sizeof(int) * numCompS);
					glGetActiveSubroutineUniformiv(id, type, i, GL_COMPATIBLE_SUBROUTINES, s);

					// Compatible Subroutines
					for (int j = 0; j < numCompS; ++j)
					{
						glGetActiveSubroutineName(id, type, s[j], 256, &len, name);
						Subroutine.functions[name] = glGetSubroutineIndex(id, type, name);
					}

					free(s);
				}

				// Add to result
				subroutines[shaders[s]->GetType()] = Subroutine;
			}
		}

		return subroutines;
	}
	
	std::map<std::string, Graphics::Attribute> Graphics::ShaderProgram::AcquireAttributes(ShaderProgramID id)
	{
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		const GLsizei bufSize = 16; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		std::map<std::string, Graphics::Attribute> attributes;

		int attributeCount;
		glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &attributeCount);
		//printf("Active Attributes: %d\n", attributeCount);

		for (GLint i = 0; i < attributeCount; i++)
		{
			glGetActiveAttrib(id, (GLuint)i, bufSize, &length, &size, &type, name);

			auto& attrib = attributes[name];
			attrib.location = glGetAttribLocation(id, name);
			attrib.type = type;
			attrib.size = size;
			attrib.name = name;

			//printf("Attribute #%d Type: %u Name: %s\n", loc, type, name);
		}

		return attributes;
	}

	// ~ Buffers ~ //

	// ~ VAO ~ //
	VAOID Graphics::VAO::Create(void)
	{
		VAOID id;
		glGenVertexArrays(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenVertexArrays()");

		return id;
	}
	void Graphics::VAO::Delete(VAOID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteVertexArrays()");

		glDeleteVertexArrays(1, &id);
	}
	void Graphics::VAO::Bind(VAOID id)
	{
		if (gl_activeVAO == id)
			return;

		glBindVertexArray(id);
		gl_activeVAO = id;
	}
	void Graphics::VAO::Unbind(void)
	{
		if (gl_activeVAO == 0)
			return;

		glBindVertexArray(0);
		gl_activeVAO = 0;
	}

	// ~ VBO ~ //
	VBOID Graphics::VBO::Create(void)
	{
		VAOID id;
		glGenBuffers(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenBuffers()");

		return id;
	}
	void Graphics::VBO::Delete(VBOID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteBuffers()");

		glDeleteBuffers(1, &id);
	}
	void Graphics::VBO::Bind(VBOID id)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	void Graphics::VBO::Unbind(void)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void Graphics::VBO::BindData(ptrdiff_t length, void* data, BufferUsage usage)
	{
		glBufferData(GL_ARRAY_BUFFER, length, data, GL_BufferUsage[(int)usage]);
	}
	void Graphics::VBO::BindSubData(int OffsetBytes, int SizeBytes, void* data)
	{
		glBufferSubData(GL_ARRAY_BUFFER, OffsetBytes, SizeBytes, data);
	}

	void Graphics::VBO::SetVertexAttribState(uint32_t bufferIndex, bool state)
	{
		if (state)
			glEnableVertexAttribArray(bufferIndex);
		else
			glDisableVertexAttribArray(bufferIndex);
	}
	void Graphics::VBO::SetVertexAttrib(uint32_t bufferIndex, int valueCount, DataType datatype, uint32_t strideSize, uint32_t strideOffset, bool normalized)
	{
		glVertexAttribPointer(bufferIndex, valueCount, GL_DataType[(int)datatype], normalized, strideSize, BUFFER_OFFSET(strideOffset));
	}
	void Graphics::VBO::SetVertexAttribDivisor(uint32_t bufferIndex, uint32_t divisor)
	{
		glVertexAttribDivisor(bufferIndex, divisor);
	}

	// ~ EBO ~ //
	EBOID Graphics::EBO::Create(void)
	{
		VAOID id;
		glGenBuffers(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenBuffers()");

		return id;
	}
	void Graphics::EBO::Delete(EBOID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteBuffers()");

		glDeleteBuffers(1, &id);
	}
	void Graphics::EBO::Bind(EBOID id)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
	void Graphics::EBO::Unbind(void)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void Graphics::EBO::BindData(ptrdiff_t length, void* data, BufferUsage usage)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, data, GL_BufferUsage[(int)usage]);
	}
	void Graphics::EBO::BindSubData(int OffsetBytes, int SizeBytes, void* data)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, OffsetBytes, SizeBytes, data);
	}

	// ~ Draw ~ //
	void Graphics::Draw::Array(DrawType type, uint32_t count)
	{
		glDrawArrays(GL_DrawType[(int)type], 0, count);
	}
	void Graphics::Draw::Indexed(DrawType type, uint32_t count)
	{
		glDrawElements(GL_DrawType[(int)type], count, GL_UNSIGNED_INT, 0);
	}
	void Graphics::Draw::ArrayInstanced(DrawType type, uint32_t count, uint32_t instanceCount)
	{
		glDrawArraysInstanced(GL_DrawType[(int)type], 0, count, instanceCount);
	}
	void Graphics::Draw::IndexedInstanced(DrawType type, uint32_t count, uint32_t instanceCount)
	{
		glDrawElementsInstanced(GL_DrawType[(int)type], count, GL_UNSIGNED_INT, 0, instanceCount);
	}

	// ~ Texture ~ //
	TextureID Graphics::Texture::Create(void)
	{
		TextureID id;
		glGenTextures(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenTextures()");

		return id;
	}
	void Graphics::Texture::Delete(TextureID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteTextures()");

		glDeleteTextures(1, &id);
	}
	void Graphics::Texture::Bind(TextureType type, TextureID textureID)
	{
		if (gl_activeTextureId == textureID)
			return;

		glBindTexture(GL_TextureType[(int)type], textureID);
		gl_activeTextureId = textureID;
	}
	void Graphics::Texture::Unbind(TextureType type)
	{
		if (gl_activeTextureId == 0)
			return;

		glBindTexture(GL_TextureType[(int)type], 0);
		gl_activeTextureId = 0;
	}
	TextureID Graphics::Texture::GetCurrentlyBound(void)
	{
		return gl_activeTextureId;
	}
	void Graphics::Texture::SetActiveLevel(TextureLevel level)
	{
		if (gl_activeTextureLayer == level)
			return;

		glActiveTexture(GL_TextureLayer[(int)level]);
		gl_activeTextureLayer = level;
	}

	void Graphics::Texture::CreateStorage(TextureType type, uint32_t levels, TextureFormat format, uint32_t width, uint32_t height)
	{
		switch (type)
		{
		case TextureType::TEX_2D:
		case TextureType::TEX_1D_ARRAY:
		case TextureType::TEX_RECT:
		case TextureType::TEX_CUBEMAP:
			glTexStorage2D(GL_TextureType[(int)type], levels, GL_TextureFormat[(int)format], width, height);
			break;
		default:
			VXL_ASSERT(false, "TextureType Type not allowed for CreateStorage");
		}
	}
	void Graphics::Texture::SetStorage(TextureType type, uint32_t width, uint32_t height, TextureChannelType channelType, TexturePixelType pixeltype, const void* pixels)
	{
		switch (type)
		{
		case TextureType::TEX_2D:
		case TextureType::TEX_1D_ARRAY:
			glTexSubImage2D(GL_TextureType[(int)type], 0, 0, 0, width, height, GL_TextureChannelType[(int)channelType], GL_TexturePixelType[(int)pixeltype], pixels);
			break;
		default:
			VXL_ASSERT(false, "TextureType Type not allowed for SetStorage(pixels)");
		}
	}
	void Graphics::Texture::SetStorage(CubemapFace type, uint32_t width, uint32_t height, TextureChannelType channelType, TexturePixelType pixeltype, const void* pixels)
	{
		VXL_ASSERT(type != CubemapFace::NONE, "SetStorage enum incorrect -> [NONE]");

		glTexSubImage2D(GL_CubemapFace[(int)type], 0, 0, 0, width, height, GL_TextureChannelType[(int)channelType], GL_TexturePixelType[(int)pixeltype], pixels);
	}

	void Graphics::Texture::GenerateMipmap(TextureType type)
	{
		glGenerateMipmap(GL_TextureType[(int)type]);
	}
	void Graphics::Texture::SetWrapping(TextureType type, TextureWrapping wrap)
	{
		glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_WRAP_S, GL_TextureWrapping[(int)wrap]);
		glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_WRAP_T, GL_TextureWrapping[(int)wrap]);
	}
	void Graphics::Texture::SetFiltering(TextureType type, TextureFilter filter, bool mipmapping)
	{
		// Mag Filter
		glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_MAG_FILTER, GL_TextureFiltering[(int)filter]);

		// Min Filter
		if (mipmapping)
		{
			if (filter == TextureFilter::LINEAR)
				glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

			// Unused Options
			// GL_LINEAR_MIPMAP_NEAREST
			// GL_NEAREST_MIPMAP_LINEAR
		}
		else
			glTexParameteri(GL_TextureType[(int)type], GL_TEXTURE_MIN_FILTER, GL_TextureFiltering[(int)filter]);
	}
	void Graphics::Texture::SetAnistropic(TextureType type, AnisotropicMode aniso)
	{
		glTexParameterf(GL_TextureType[(int)type], GL_TEXTURE_MAX_ANISOTROPY, min(GLMaxAnisotropy, (float)aniso));
	}
	void Graphics::Texture::SetBorderColor(TextureType type, const Color4F& color)
	{
		float c[4] = { color.r, color.g, color.b, color.a };
		glTexParameterfv(GL_TextureType[(int)type], GL_TEXTURE_BORDER_COLOR, c);
	}
	void Graphics::Texture::SetPixelPackAlignment(PixelAlignment align)
	{
		if (gl_pixelPackAlignment == align)
			return;

		glPixelStorei(GL_PACK_ALIGNMENT, (int)align);
		// affects glreadpixels and texture unpacking ex: glTexImage2D

		gl_pixelPackAlignment = align;
	}
	void Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment align)
	{
		if (gl_pixelUnpackAlignment == align)
			return;

		glPixelStorei(GL_UNPACK_ALIGNMENT, (int)align);
		// affects glreadpixels and texture unpacking ex: glTexImage2D

		gl_pixelUnpackAlignment = align;
	}
	// Note this is an expensive function, plz use the FBO Readpixels if at all possible, its 50 times cheaper than this
	RawArray<uint8_t> Graphics::Texture::ReadPixels(const Vxl::BaseTexture& texture, int x, int y, int w, int h)
	{
		RawArray<uint8_t> Array;
		Array.Allocate(w * h * texture.GetChannelCount());
		glGetTextureSubImage(
			texture.GetID(), 0,
			x, y, 0,
			w, h, 1,
			GL_TextureChannelType[(int)texture.GetChannelType()],
			GL_TexturePixelType[(int)texture.GetPixelType()],
			w * h * texture.GetChannelCount(),
			Array.start
		);

		return Array;
	}

	// ~ Renderbuffer ~ //
	RenderBufferID Graphics::RenderBuffer::Create(void)
	{
		RenderBufferID id;
		glGenRenderbuffers(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenRenderbuffers()");

		return id;
	}
	void Graphics::RenderBuffer::Delete(RenderBufferID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glGenRenderbuffers()");

		glDeleteRenderbuffers(1, &id);
	}
	void Graphics::RenderBuffer::Bind(RenderBufferID id)
	{
		if (gl_activeBufferId == id)
			return;

		glBindRenderbuffer(GL_RENDERBUFFER, id);
		gl_activeBufferId = id;
	}
	void Graphics::RenderBuffer::Unbind(void)
	{
		if (gl_activeBufferId == 0)
			return;

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		gl_activeBufferId = 0;
	}
	void Graphics::RenderBuffer::SetStorage(TextureFormat format, int width, int height)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_TextureFormat[(int)format], width, height);
	}
	RenderBufferID Graphics::RenderBuffer::GetCurrentlyBound(void)
	{
		return gl_activeBufferId;
	}

	// ~ Framebuffer Object ~ //
	FramebufferObjectID Graphics::FramebufferObject::Create(void)
	{
		FramebufferObjectID id;
		glGenFramebuffers(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenFramebuffers()");

		return id;
	}
	void Graphics::FramebufferObject::Delete(FramebufferObjectID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteFramebuffers()");

		glDeleteFramebuffers(1, &id);
	}
	void Graphics::FramebufferObject::Bind(FramebufferObjectID id)
	{
		if (gl_activeFBO == id)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, id);
		gl_activeFBO = id;
	}
	void Graphics::FramebufferObject::DrawBuffers(std::vector<uint32_t> attachments)
	{
		for (auto& a : attachments)
			a += GL_COLOR_ATTACHMENT0;

		glDrawBuffers((GLsizei)attachments.size(), attachments.data());
	}
	//	void Graphics::FramebufferObject::DrawBuffers(uint32_t attachmentCount)
	//	{
	//		glDrawBuffers(attachmentCount, GL_ColorAttachments);
	//	}
	void Graphics::FramebufferObject::DrawBuffer(uint32_t attachmentIndex)
	{
		GLenum arr[] = { GL_COLOR_ATTACHMENT0 + attachmentIndex };
		glDrawBuffers(1, arr);
	}

	void Graphics::FramebufferObject::Unbind()
	{
		if (gl_activeFBO == 0)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		gl_activeFBO = 0;
	}
	bool Graphics::FramebufferObject::CheckStatus(void)
	{
		// Check if FBO was Created Correctly
		GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		switch (e) {

		case GL_FRAMEBUFFER_UNDEFINED:
			Logger.error("FBO Undefined");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			Logger.error("FBO Incomplete Attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			Logger.error("FBO Missing Attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			Logger.error("FBO Incomplete Draw Buffer");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			Logger.error("FBO Unsupported");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			//FBO is OK
			break;

		default:
			//Unknown FBO
			Logger.error("FBO Error, Unknown: " + std::to_string(e));
			break;
		}

		// Return true if framebuffer complete
		return (e == GL_FRAMEBUFFER_COMPLETE) ? true : false;
	}
	FramebufferObjectID Graphics::FramebufferObject::GetCurrentlyBound(void)
	{
		return gl_activeFBO;
	}

	void Graphics::FramebufferObject::AttachRenderTexture(const Vxl::RenderTexture& texture, uint32_t attachmentIndex)
	{
		VXL_ASSERT(attachmentIndex < (uint32_t)GLMaxFBOColorAttachments, "attachmentIndex too high for AttachRenderTexture()");
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, texture.GetID(), 0);
	}
	void Graphics::FramebufferObject::AttachRenderTextureAsDepth(const Vxl::RenderTexture& texture)
	{
		GLenum AttachmentType;
		switch (texture.GetChannelType())
		{
		case TextureChannelType::STENCIL:
			AttachmentType = GL_STENCIL_ATTACHMENT;
			break;
		case TextureChannelType::DEPTH:
			AttachmentType = GL_DEPTH_ATTACHMENT;
			break;
		case TextureChannelType::DEPTH_STENCIL:
			AttachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			VXL_ERROR("Invalid ChannelType for FBO Depth Attachment");
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, GL_TEXTURE_2D, texture.GetID(), 0);
	}
	void Graphics::FramebufferObject::DetachRenderTexture(uint32_t attachmentIndex)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, 0, 0);
	}
	void Graphics::FramebufferObject::AttachRenderBuffer(const Vxl::RenderBuffer& texture, uint32_t attachmentIndex)
	{
		VXL_ASSERT(attachmentIndex < (uint32_t)GLMaxFBOColorAttachments, "attachmentIndex too high for AttachRenderTexture()");
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, texture.GetID());
	}
	void Graphics::FramebufferObject::AttachRenderBufferAsDepth(const Vxl::RenderBuffer& texture)
	{
		GLenum AttachmentType;
		switch (texture.GetChannelType())
		{
		case TextureChannelType::STENCIL:
			AttachmentType = GL_STENCIL_ATTACHMENT;
			break;
		case TextureChannelType::DEPTH:
			AttachmentType = GL_DEPTH_ATTACHMENT;
			break;
		case TextureChannelType::DEPTH_STENCIL:
			AttachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			VXL_ERROR("Invalid ChannelType for FBO Depth Attachment");
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, AttachmentType, GL_RENDERBUFFER, texture.GetID());
	}
	void Graphics::FramebufferObject::DetachRenderBuffer(uint32_t attachmentIndex)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, 0);
	}

	RawArray<uint8_t> Graphics::FramebufferObject::ReadPixels(const Vxl::RenderTexture& texture, uint32_t attachmentIndex, int x, int y, int w, int h)
	{
		if(attachmentIndex != -1)
			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		RawArray<uint8_t> Array;
		Array.Allocate(w * h * texture.GetChannelCount());
		glReadPixels(
			x, y, w, h,
			GL_TextureChannelType[(int)texture.GetChannelType()],
			GL_TexturePixelType[(int)texture.GetPixelType()],
			Array.start
		);

		return Array;
	}
	RawArray<uint8_t> Graphics::FramebufferObject::ReadPixels(const Vxl::RenderBuffer& texture, uint32_t attachmentIndex, int x, int y, int w, int h)
	{
		if(attachmentIndex != -1)
			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		RawArray<uint8_t> Array;
		Array.Allocate(w * h * texture.GetChannelCount());
		glReadPixels(
			x, y, w, h,
			GL_TextureChannelType[(int)texture.GetChannelType()],
			GL_TexturePixelType[(int)texture.GetPixelType()],
			Array.start
		);
		
		return Array;
	}
	void Graphics::FramebufferObject::BlitColor(FramebufferObjectID source, FramebufferObjectID destination, int width, int height, uint32_t srcAttachment, uint32_t destAttachment)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + srcAttachment);

		// Actually ends up breaking the blit?
		//	const GLenum bufs[] = { destAttachment };
		//	glDrawBuffers(1, bufs);

		glBlitFramebuffer(
			0, 0, width, height,
			0, 0, width, height,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);

		// Revert to default // 500 IQ workaround for opengl
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_activeFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_activeFBO); 

		// ? Is this necessary ?
		glReadBuffer(GL_FRONT);
	}
	void Graphics::FramebufferObject::BlitDepth(FramebufferObjectID source, FramebufferObjectID destination, int width, int height)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination);

		glBlitFramebuffer(
			0, 0, width, height,
			0, 0, width, height,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);

		// Revert to default // 500 IQ workaround for opengl
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gl_activeFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gl_activeFBO); 
	}

	// ~ UBO ~ //
	UBOID Graphics::UBO::Create(uint32_t slot, uint32_t totalBytes, BufferUsage usage)
	{
		UBOID id;
		glGenBuffers(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenBuffers()");

		// Bind
		Bind(id);

		// Allocate size
		glBufferData(GL_UNIFORM_BUFFER, totalBytes, NULL, GL_BufferUsage[(int)usage]);

		// Set to slot
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, id);

		return id;
	}
	void Graphics::UBO::Delete(UBOID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteBuffers()");

		glDeleteBuffers(1, &id);
	}
	void Graphics::UBO::Bind(UBOID id)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, id);
	}
	void Graphics::UBO::Unbind(void)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void Graphics::UBO::UpdateBuffer(void* buffer, uint32_t totalBytes, uint32_t offset)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, totalBytes, buffer);
	}

	// ~ Queries ~ //
	QueryID Graphics::Query::Create(void)
	{
		QueryID id;
		glGenQueries(1, &id);

		VXL_ASSERT(id != -1, "GL ERROR: glGenQueries()");

		return id;
	}
	void Graphics::Query::Delete(QueryID id)
	{
		VXL_ASSERT(id != -1, "GL ERROR: glDeleteQueries()");

		glDeleteQueries(1, &id);
	}

	std::map<Graphics::Query::Type, bool> gl_QueryStatus;

	void Graphics::Query::Start(QueryID id, Type type)
	{
		VXL_ASSERT(!gl_QueryStatus[type], "GL ERROR: Query is already Active, glBeginQuery()");

		glBeginQuery(GL_QueryType[(int)type], id);
		gl_QueryStatus[type] = true;
	}
	void Graphics::Query::End(Type type)
	{
		VXL_ASSERT(gl_QueryStatus[type], "GL ERROR: Query isn't Active, glEndQuery()");

		glEndQuery(GL_QueryType[(int)type]);
		gl_QueryStatus[type] = false;
	}
	bool Graphics::Query::CheckFinished(QueryID id)
	{
		int ready = 0;
		glGetQueryObjectiv(id, GL_QUERY_RESULT_AVAILABLE, &ready);
		return ready > 0;
	}
	uint64_t Graphics::Query::GetResult(QueryID id)
	{
		uint64_t result;
		glGetQueryObjectui64v(id, GL_QUERY_RESULT, &result);
		return result;
	}
}