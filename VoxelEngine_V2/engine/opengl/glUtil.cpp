// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "glUtil.h"

#include "../utilities/logger.h"
#include "../Math/Color.h"
#include "../math/Vector4.h"
#include "../textures/Texture.h"
#include "../utilities/Macros.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

namespace Vxl
{
	void glInfo::Setup()
	{
		// Acquire openglversion
		glGetIntegerv(GL_MAJOR_VERSION, &GLVersionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &GLVersionMinor);

		// Acquire Vendor
		std::string VendorStr = glUtil::getVendor();
		std::transform(VendorStr.begin(), VendorStr.end(), VendorStr.begin(), ::tolower);
		if (VendorStr.find("nvidia") != -1)
			Vendor = VendorType::NVIDIA;
		else if (VendorStr.find("ati") != -1)
			Vendor = VendorType::ATI;
		else if (VendorStr.find("radeon") != -1)
			Vendor = VendorType::RADEON;
		else if (VendorStr.find("intel") != -1)
			Vendor = VendorType::INTEL;

		// Acquire GPU info
		Gpu_Renderer = std::string(glUtil::getRendererVersion());
		Gpu_OpenGLVersion =  std::string(glUtil::getOpenGLVersion());
		Gpu_Vendor = std::string(glUtil::getVendor());

		// Acquire Restrictions
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MAX_ANISOTROPY);
	}

	bool glUtil::initGlew()
	{
		if (gl3wInit())
		{
			Logger.error("failed to initialize OpenGL");
			std::system("pause");
			return false;
		}

		// Setup opengl info about context
		glInfo.Setup();

		if (!gl3wIsSupported(glInfo.GLVersionMajor, glInfo.GLVersionMinor))
		{
			Logger.error("OpenGL " + std::to_string(glInfo.GLVersionMajor) + '.' + std::to_string(glInfo.GLVersionMinor) + " not supported");
			std::system("pause");
			return false;
		}

		return true;
	}

	// Init GL Hints
	Cull_Type			_cullmode = Cull_Type::NONE;
	bool				_blendState = false;
	Blend_Source		_blendsrc = Blend_Source::NONE;
	Blend_Destination	_blenddest = Blend_Destination::NONE;
	Blend_Equation		_blendequation = Blend_Equation::NONE;
	Depth_Pass_Rule		_rule = Depth_Pass_Rule::NONE;
	bool				_depthState = false;
	bool				_depthMask = true;
	bool				_wireframe = false;
	GLsizei				_viewport[4] = { -1, -1, -1, -1 };

	void glUtil::initHints()
	{
		// Reset
		_cullmode = Cull_Type::NONE;
		_blendState = false;
		_blendsrc = Blend_Source::NONE;
		_blenddest = Blend_Destination::NONE;
		_blendequation = Blend_Equation::NONE;
		_rule = Depth_Pass_Rule::NONE;
		_depthState = false;
		_depthMask = true;
		_wireframe = false;
		for(int i = 0; i < 4; i++)
			_viewport[i] = -1;

		// Set Default Cull Mode
		cullMode(Cull_Type::COUNTER_CLOCKWISE);
		// Set Default Blend Mode
		blendState(true);
		blendMode(Blend_Source::SRC_ALPHA, Blend_Destination::ONE_MINUS_SRC_ALPHA);
		blendEquation(Blend_Equation::FUNC_ADD);
		// Set Default Depth Test
		depthState(true);
		depthTest(Depth_Pass_Rule::LESS_OR_EQUAL);
		// Set Default Wireframe Mode
		wireframe(false);

		// Cubemap edges are hidden
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	// Get Version Data
	const char* glUtil::getRendererVersion()
	{
		return reinterpret_cast<char const*>(glGetString(GL_RENDERER)); // get renderer string (GLubyte*)
	}
	const char* glUtil::getOpenGLVersion()
	{
		return reinterpret_cast<char const*>(glGetString(GL_VERSION)); // version as a string (GLubyte*)
	}
	const char* glUtil::getVendor()
	{
		return reinterpret_cast<char const*>(glGetString(GL_VENDOR)); // vendor as a string (GLubyte*)
	}

	// Get Memory
	int glUtil::GetGPUMem_TotalKB()
	{
		if (glInfo.Vendor == VendorType::NVIDIA)
		{
			GLint total_mem_kb = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
			return total_mem_kb;
		}
		else
			return -1;
	}
	int glUtil::GetGPUMem_CurrentKB()
	{
		if (glInfo.Vendor == VendorType::NVIDIA)
		{
			GLint cur_avail_mem_kb = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
			return cur_avail_mem_kb;
		}
		else
			return -1;
	}

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
			std::system("PAUSE");
		}
		else
		{
			// For some reason it hates how im storing my VBOs/EBOs
			//Logger.log(message);
		}
	}

	// Gives errors in random locations, cannot use for now
	void glUtil::initOpenglErrorCallback()
	{
#if defined(GLOBAL_ERROR_CALLBACK) && !defined(GLOBAL_USE_GLNAMES)
		//GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		//if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			UsingErrorCallback = true;
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugCallback, stderr);
			//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
		}
#endif
	}

	// Check if string name matches GL type
	bool glUtil::VeryifyDataType(const std::string& name, GLenum type)
	{
		// Bool
		if (name.compare("bool") == 0 && type == GL_BOOL)
			return true;
		// Float
		if (name.compare("float") == 0 && type == GL_FLOAT)
			return true;
		// Vec2
		if (name.compare("class Vxl::Vector2") == 0 && type == GL_FLOAT_VEC2)
			return true;
		// Vec3 - Color3F
		if ((name.compare("class Vxl::Vector3") == 0 || name.compare("class Vxl::Color3F") == 0) && type == GL_FLOAT_VEC3)
			return true;
		// Vec4
		if ((name.compare("class Vxl::Vector4") == 0 || name.compare("class Vxl::Color4F") == 0) && type == GL_FLOAT_VEC4)
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

		VXL_ASSERT(false, "Could not verify GLSL Data Type");
		return false;
	}

	// Set OpenGL Name
	void glUtil::setGLName(glNameType identifier, GLuint id, const std::string &label)
	{
#if defined(GLOBAL_USE_GLNAMES) && !defined(GLOBAL_ERROR_CALLBACK)
		//
		if(GLNameMaxLength == -1)
			glGetIntegerv(GL_MAX_LABEL_LENGTH, &GLNameMaxLength);

		std::string labelEdit = label + " (" + std::to_string(id) + ")";
		VXL_ASSERT(labelEdit.size() < GLNameMaxLength, "GLName is too big");
		glObjectLabel((GLenum)identifier, id, static_cast<GLsizei>(labelEdit.size()), labelEdit.c_str());
		//
#endif
	}

	// Get Format Type from channel count
	Channel_Type glUtil::getChannelType(int ChannelCount)
	{
		switch (ChannelCount)
		{
		default:
		case 0:
			return Channel_Type::NONE;
		case 1:
			return Channel_Type::R;
		case 2:
			return Channel_Type::RG;
		case 3:
			return Channel_Type::RGB;
		case 4:
			return Channel_Type::RGBA;
		}

		VXL_ASSERT(false, "Invalid Channel Count");
		return Channel_Type::NONE;
	}
	unsigned int glUtil::getChannelCount(Channel_Type type)
	{
		// Solve channel count based on channel type
		switch (type)
		{
		case Channel_Type::NONE:
			return 0;
		case Channel_Type::R:
			return 1;
		case Channel_Type::RG:
			return 2;
		case Channel_Type::RGB:
		case Channel_Type::BGR:
			return 3;
		case Channel_Type::RGBA:
		case Channel_Type::BGRA:
			return 4;
			// Special Case [Cannot figure out channel count based on this information alone]
		case Channel_Type::DEPTH:
		case Channel_Type::DEPTH_STENCIL:
			return -1;
		}
		
		VXL_ASSERT(false, "Invalid Channel Type");
		return 0;
	}

	unsigned int glUtil::getChannelCount(Format_Type format)
	{
		// Special exceptions
		switch (format)
		{
			// 1 Channel
		case Format_Type::R8:
		case Format_Type::R8_SNORM:
		case Format_Type::R16:
		case Format_Type::R16_SNORM:
		case Format_Type::R16F:
			return 1;
			// 2 Channels
		case Format_Type::RG8:
		case Format_Type::RG8_SNORM:
		case Format_Type::RG16:
		case Format_Type::RG16_SNORM:
		case Format_Type::RG16F:
		case Format_Type::DEPTH16:// Special
			return 2;
			// 3 Channels
		case Format_Type::RGB8:
		case Format_Type::RGB8_SNORM:
		case Format_Type::RGB16:
		case Format_Type::RGB16_SNORM:
		case Format_Type::RGB16F:
		case Format_Type::R11F_G11F_B10F:// Special
		case Format_Type::SRGB8:// Special
		case Format_Type::DEPTH24:// Special
			return 3;
			// 4 ChannelS
		case Format_Type::RGBA8:
		case Format_Type::RGBA8_SNORM:
		case Format_Type::RGBA16:
		case Format_Type::RGBA16_SNORM:
		case Format_Type::RGBA16F:
		case Format_Type::SRGBA8:// Special
		case Format_Type::DEPTH32:// Special
		case Format_Type::DEPTH32F:// Special
		case Format_Type::DEPTH24_STENCIL8:// Special
			return 4;
		case Format_Type::DEPTH32F_STENCIL8:
			return 8; // 64 bits total (24 bits padding for alignment)
		}

		VXL_ASSERT(false, "Invalid Format Type");
		return 0;
	}

	//
	uint32_t glUtil::getShaderDataTypeValueCount(ShaderDataType data)
	{
		switch (data)
		{
		case ShaderDataType::BOOL:
		case ShaderDataType::INT:
		case ShaderDataType::UNSIGNED_INT:
		case ShaderDataType::FLOAT:
		case ShaderDataType::DOUBLE:
			return 1;
		case ShaderDataType::VEC2:
			return 2;
		case ShaderDataType::VEC3:
			return 3;
		case ShaderDataType::VEC4:
		case ShaderDataType::MAT2:
			return 4;
		case ShaderDataType::MAT3:
			return 9;
		case ShaderDataType::MAT4:
			return 16;
		default:
			VXL_ASSERT(false, "Invalid ShaderDataType");
			return 0;
		}
	}
	uint32_t glUtil::getShaderDataTypeSize(ShaderDataType data)
	{
		switch (data)
		{
		case ShaderDataType::BOOL:
		case ShaderDataType::INT:
		case ShaderDataType::UNSIGNED_INT:
		case ShaderDataType::FLOAT:
			return 4;
		case ShaderDataType::DOUBLE:
		case ShaderDataType::VEC2:
			return 8;
		case ShaderDataType::VEC3:
			return 12;
		case ShaderDataType::VEC4:
		case ShaderDataType::MAT2:
			return 16;
		case ShaderDataType::MAT3:
			return 36;
		case ShaderDataType::MAT4:
			return 64;
		default:
			VXL_ASSERT(false, "Invalid ShaderDataType");
			return 0;
		}
	}

	// Get Pixel/Channel Data From DepthFormat
	void glUtil::getPixelChannelData(DepthFormat_Type format, Channel_Type& channelType, Pixel_Type& pixelType)
	{
		switch (format)
		{
		case DepthFormat_Type::STENCIL8:
			channelType = Channel_Type::STENCIL;
			pixelType = Pixel_Type::UNSIGNED_BYTE;
			return;
		case DepthFormat_Type::DEPTH16:
			channelType = Channel_Type::DEPTH;
			pixelType = Pixel_Type::UNSIGNED_SHORT;
			return;
		case DepthFormat_Type::DEPTH24:
			channelType = Channel_Type::DEPTH;
			pixelType = Pixel_Type::UNSIGNED_INT_8_8_8_8;
			return;
		case DepthFormat_Type::DEPTH24_STENCIL8:
			channelType = Channel_Type::DEPTH_STENCIL;
			pixelType = Pixel_Type::UNSIGNED_INT_8_8_8_8;
			return;
		case DepthFormat_Type::DEPTH32:
		case DepthFormat_Type::DEPTH32F:
			channelType = Channel_Type::DEPTH;
			pixelType = Pixel_Type::UNSIGNED_INT;
			return;
		case DepthFormat_Type::DEPTH32F_STENCIL8:
			channelType = Channel_Type::DEPTH_STENCIL;
			pixelType = Pixel_Type::UNSIGNED_INT;
			return;
		}

		VXL_ASSERT(false, "Invalid DepthFormat Type");
	}


	void glUtil::clearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	void glUtil::clearColor(const Color3F& c, float a)
	{
		glClearColor(c.r, c.g, c.b, 1.0f);
	}
	void glUtil::clearColor(const Color4F& c)
	{
		glClearColor(c.r, c.g, c.b, c.a);
	}
	void glUtil::clearDepth(float f)
	{
		VXL_ASSERT(f >= 0.0f && f <= 1.0f, "Depth Value must be between 0 and 1 inclusif");
		glClearDepth(f);
	}
	void glUtil::clearStencil(int f)
	{
		VXL_ASSERT(f >= 0.0f && f <= 1.0f, "Stencil Value must be between 0 and 1 inclusif");
		glClearStencil(f);
	}

	void glUtil::clearBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void glUtil::clearBuffer(UINT bitCode)
	{
		glClear(bitCode);
	}
	void glUtil::clearBuffer(Buffer_Bit_Type a, Buffer_Bit_Type b, Buffer_Bit_Type c, Buffer_Bit_Type d)
	{
		glClear(GLbitfield(a) | GLbitfield(b) | GLbitfield(c) | GLbitfield(d));
	}

	void glUtil::cullMode(Cull_Type cull)
	{
		if (_cullmode != cull)
		{
			switch (cull)
			{
			case Cull_Type::NO_CULL:
				glDisable(GL_CULL_FACE);
				break;

			case Cull_Type::COUNTER_CLOCKWISE:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				break;

			case Cull_Type::CLOCKWISE:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CW);
				break;
			default:
				VXL_ASSERT(FALSE, "Invalud CullType Mode");
				break;
			}

		}

		_cullmode = cull;
	}

	// Blend Mode (affects rgb of colors based on alpha/rgb of source and destination pixels in shaders)
	void glUtil::blendState(bool state)
	{
		if (_blendState == state)
			return;

		if(state)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		_blendState = state;
	}
	
	void glUtil::blendMode(Blend_Source src = Blend_Source::SRC_ALPHA, Blend_Destination dest = Blend_Destination::ONE_MINUS_SRC_ALPHA)
	{
		if(_blendsrc != src || _blenddest != dest)
			glBlendFunc((GLenum)src, (GLenum)dest);

		_blendsrc = src;
		_blenddest = dest;
	}
	
	void glUtil::blendEquation(Blend_Equation equation)
	{
		if(_blendequation != equation)
			glBlendEquation((GLenum)equation);

		_blendequation = equation;
	}

	void glUtil::depthState(bool state)
	{
		if (_depthState == state)
			return;

		if(state)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		_depthState = state;
	}
	// Depth Test (What depth value will overwrite the existing one)
	void glUtil::depthTest(Depth_Pass_Rule Rule = Depth_Pass_Rule::LESS_OR_EQUAL)
	{
		if (_rule != Rule)
			glDepthFunc((GLenum)Rule);

		_rule = Rule;
	}
	void glUtil::depthMask(bool state)
	{
		if(_depthMask != state)
			glDepthMask(state);

		_depthMask = state;
	}

	// Wireframe Mode
	void glUtil::wireframe(bool state)
	{
		if (_wireframe != state)
		{
			if (state)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		_wireframe = state;
	}

	// viewport
	void glUtil::viewport(GLsizei x, GLsizei y, GLsizei w, GLsizei h)
	{
		if(_viewport[0] != x || _viewport[1] != y || _viewport[2] != w || _viewport[3] != h)
			glViewport(x, y, w, h);

		_viewport[0] = x;
		_viewport[1] = y;
		_viewport[2] = w;
		_viewport[3] = h;
	}

	// BUFFERS //
	GLuint glUtil::generateVAO()
	{
		GLuint VAO = 0;
		glGenVertexArrays(1, &VAO);
		return VAO;
	}
	void glUtil::deleteVAO(GLuint& VAO)
	{
		glDeleteVertexArrays(1, &VAO);
	}
	void glUtil::bindVAO(GLuint VAO)
	{
		glBindVertexArray(VAO);
	}
	void glUtil::unbindVAO()
	{
		glBindVertexArray(0);
	}

	void glUtil::bindVBO(GLuint VBO)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	}
	void glUtil::bindVBOSubData(GLuint VBO, GLsizei Offset, GLsizei Size, GLvoid* Data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, Offset, Size, Data);
	}
	void glUtil::bindArray(GLuint VBO, GLsizeiptr length, GLvoid* data, GLenum usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, length, data, usage);
	}
	void glUtil::setVertexAttrib(GLuint bufferIndex, int valueCount, Data_Type dataType, GLuint strideSize, GLuint strideOffset, bool normalized)
	{
		glEnableVertexAttribArray(bufferIndex);
		glVertexAttribPointer(bufferIndex, valueCount, (GLenum)dataType, normalized, strideSize, BUFFER_OFFSET(strideOffset));
	}
	void glUtil::setVertexAttribDivisor(GLuint bufferIndex, GLuint divisor)
	{
		glVertexAttribDivisor(bufferIndex, divisor);
	}
	//	// notice takes 4 attribute array slots (starting from given index)
	//	void glUtil::setVertexAttribInstancing(GLuint bufferIndex)
	//	{
	//		glEnableVertexAttribArray(bufferIndex + 0);
	//		glVertexAttribPointer(bufferIndex + 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)0);
	//		glEnableVertexAttribArray(bufferIndex + 1);
	//		glVertexAttribPointer(bufferIndex + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(sizeof(Vector4)));
	//		glEnableVertexAttribArray(bufferIndex + 2);
	//		glVertexAttribPointer(bufferIndex + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(2 * sizeof(Vector4)));
	//		glEnableVertexAttribArray(bufferIndex + 3);
	//		glVertexAttribPointer(bufferIndex + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(3 * sizeof(Vector4)));
	//	
	//		glVertexAttribDivisor(bufferIndex + 0, 1);
	//		glVertexAttribDivisor(bufferIndex + 1, 1);
	//		glVertexAttribDivisor(bufferIndex + 2, 1);
	//		glVertexAttribDivisor(bufferIndex + 3, 1);
	//	}

	void glUtil::bindEBO(GLuint EBO)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	}
	void glUtil::bindIndices(GLuint EBO, GLsizeiptr length, GLvoid* data, GLenum usage)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, data, usage);
	}

	void glUtil::drawArray(Draw_Type type, GLuint count)
	{
		glDrawArrays((GLenum)type, 0, count);
	}
	void glUtil::drawArrayIndexed(Draw_Type type, GLuint count)
	{
		glDrawElements((GLenum)type, count, GL_UNSIGNED_INT, 0);
	}

	// Draw Type
	bool glUtil::isDrawTypeTriangles(Draw_Type type)
	{
		return (
			type == Draw_Type::TRIANGLES ||
			type == Draw_Type::TRIANGLES_ADJACENCY ||
			type == Draw_Type::TRIANGLE_FAN ||
			type == Draw_Type::TRIANGLE_STRIP ||
			type == Draw_Type::TRIANGLE_STRIP_ADJACENCY
			);
	}
	bool glUtil::isDrawTypeLines(Draw_Type type)
	{
		return (
			type == Draw_Type::LINES ||
			type == Draw_Type::LINES_ADJACENCY ||
			type == Draw_Type::LINE_LOOP ||
			type == Draw_Type::LINE_STRIP ||
			type == Draw_Type::LINE_STRIP_ADJACENCY
			);
	}
	bool glUtil::isDrawTypePoints(Draw_Type type)
	{
		return (
			type == Draw_Type::POINTS
			);
	}

	// TEXTURES //
	Active_Texture activeSlot = Active_Texture::LEVEL0;
	void glUtil::setActiveTexture(Active_Texture level)
	{
		if (activeSlot == level)
			return;

		glActiveTexture(GLenum(level));

		activeSlot = level;
	}
	void glUtil::setActiveTexture(int level)
	{
		Active_Texture _level = Active_Texture(GL_TEXTURE0 + level);
		setActiveTexture(_level);
	}
	void glUtil::setActiveTexture(unsigned int level)
	{
		glActiveTexture(GL_TEXTURE0 + level);
	}

	void glUtil::bindTexture(Texture_Type type, GLuint& texture_id)
	{
		glBindTexture((GLenum)type, texture_id);
	}

	void glUtil::unbindTexture(Texture_Type type)
	{
		glBindTexture((GLenum)type, 0);
	}

	// MISC //
	std::string glUtil::shaderTypeToString(const ShaderType& type)
	{
		switch (type)
		{
		case ShaderType::VERTEX:
			return "VERTEX";
		case ShaderType::FRAGMENT:
			return "FRAGMENT";
		case ShaderType::GEOMETRY:
			return "GEOMETRY";
		case ShaderType::TESSELATION_CONTROL:
			return "TESSELATION_CONTROL";
		case ShaderType::TESSELATION_EVALUATION:
			return "TESSELATION_EVALUATION";
		case ShaderType::COMPUTE:
			return "COMPUTE";
		default:
			return "ERROR";
		}
	}

	// GET OPENGL INFO //
	GLint glUtil::GetMaxUniformBufferBindings()
	{
		static GLint MaxBindingPoints = -1;

		if(MaxBindingPoints == -1)
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MaxBindingPoints);

		return MaxBindingPoints;
	}
	GLint glUtil::GetMaxFBOColorAttachments()
	{
		static GLint maxAttach = -1;

		if(maxAttach == -1)
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);

		return maxAttach;
	}
}
