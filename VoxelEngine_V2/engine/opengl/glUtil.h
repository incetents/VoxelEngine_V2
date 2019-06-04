// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"

#include <string>

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

namespace Vxl
{
	class Color3F;
	class Color4F;
	enum class Channel_Type;

	enum class VendorType
	{
		UNKNOWN,
		NVIDIA, 
		ATI,
		INTEL
	};

	namespace glUtil
	{
		// OpenGL Version
		static int GLVersionMajor = 0;
		static int GLVersionMinor = 0;
		// Vendor Information
		static VendorType Vendor = VendorType::UNKNOWN;

		// Setup Glew for OpenGL
		bool initGlew();

		// Init hints
		void initHints();

		// Check Version Wrapped
		void CheckVersion();
		// Get Version Data
		const char* getRendererVersion();
		const char* getOpenGLVersion();
		const char* getVendor();

		// Get Memory
		int GetGPUMem_TotalKB();
		int GetGPUMem_CurrentKB();

		// Error Message
		static bool UsingErrorCallback = false;
		void initOpenglErrorCallback();

		// Set OpenGL Name
		static int GLNameMaxLength = -1;
		void setGLName(glNameType identifier, GLuint name, const std::string &label);

		// Get Format Type from channel count
		Channel_Type getFormat(int ChannelCount);
		unsigned int getFormatCount(Channel_Type type);

		// Clear Setup
		void clearColor(float r, float g, float b, float a = 1.0f);
		void clearColor(const Color3F& c, float a = 1.0f);
		void clearColor(const Color4F& c);
		void clearDepth(float);
		void clearStencil(int);
		// Clear Buffer
		void clearBuffer();
		void clearBuffer(UINT bitCode);
		void clearBuffer(Buffer_Bit_Type a, Buffer_Bit_Type b = Buffer_Bit_Type::NONE, Buffer_Bit_Type c = Buffer_Bit_Type::NONE, Buffer_Bit_Type d = Buffer_Bit_Type::NONE);
		
		// Cull Mode
		void cullMode(Cull_Type);

		// Blend Data
		void blendState(bool state);
		void blendMode(Blend_Source src, Blend_Destination dst);
		void blendEquation(Blend_Equation equation);

		// Depth Rule
		void depthState(bool state);
		void depthMask(bool state);
		void depthTest(Depth_Pass_Rule Rule);

		// Wireframe
		void wireframe(bool state);

		// viewport
		void viewport(GLsizei x, GLsizei y, GLsizei w, GLsizei h);

		// BUFFERS //
		// VAO
		GLuint generateVAO();
		void deleteVAO(GLuint& VAO);
		void bindVAO(GLuint VAO);
		void unbindVAO();

		// VBO
		void bindVBO(GLuint VBO);
		void bindVBOSubData(GLuint VBO, GLsizei Offset, GLsizei Size, GLvoid* Data);
		// VBO - Array
		void bindArray(GLuint VBO, GLsizeiptr length, GLvoid * data, GLenum usage);
		void setVertexAttrib(GLuint bufferIndex, int valueCount, Data_Type dataType = Data_Type::FLOAT, GLuint m_strideSize = 0, GLuint m_strideOffset = 0);
		void setVertexAttribInstancing(GLuint bufferIndex);
		// EBO - Index
		void bindEBO(GLuint EBO);
		void bindIndices(GLuint EBO, GLsizeiptr length, GLvoid * data, GLenum usage);

		// Draw Buffer (VAO needs to be bound before this call)
		void drawArray(Draw_Type type, GLuint count);
		void drawArrayIndexed(Draw_Type type, GLuint count);

		// Draw Type
		bool isDrawTypeTriangles(Draw_Type type);
		bool isDrawTypeLines(Draw_Type type);
		bool isDrawTypePoints(Draw_Type type);

		// TEXTURES //
		void setActiveTexture(Active_Texture level);
		void setActiveTexture(int level);
		void setActiveTexture(unsigned int level);

		void bindTexture(Texture_Type type, GLuint& texture_id);
		void unbindTexture(Texture_Type type);

		// MISC //
		std::string shaderTypeToString(const ShaderType& type);

		// GET OPENGL INFO //
		GLint GetMaxUniformBufferBindings();
		GLint GetMaxFBOColorAttachments();
	}
}