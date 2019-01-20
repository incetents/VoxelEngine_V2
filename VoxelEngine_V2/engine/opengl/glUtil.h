// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"

#include <string>

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

namespace Vxl
{
	class Color3F;
	class Color4F;

	namespace glUtil
	{
		// Setup Glew for OpenGL
		bool initGlew();

		// Init hints
		void initHints();

		// Check Version Wrapped
		void CheckVersion();
		// Get Version Data
		const char* getRendererVersion();
		const char* getOpenGLVersion();

		// Get Format Type from channel count
		Format_Type getFormatFloat(int ChannelCount);
		Format_Type getFormatInt(int ChannelCount);

		// Clear Color
		void clearColor();
		void clearColor(float r, float g, float b, float a = 1.0f);
		void clearColor(int r, int g, int b, int a = 255);
		void clearColor(const Color3F& c);
		void clearColor(const Color4F& c);
		// Clear Buffer
		void clearBuffer();
		void clearBuffer(Buffer_Bit_Type);
		void clearBuffer(Buffer_Bit_Type, Buffer_Bit_Type);
		// Clear Special
		void clearDepth(float);
		void clearStencil(int);

		// Cull Mode
		void cullMode(Cull_Type);

		// Blend Data
		void blendMode(Blend_Source src, Blend_Destination dst);

		// Depth Rule
		void depthTest(Depth_Pass_Rule Rule);

		// Wireframe Data
		void wireframe(bool state);
		void wireframeWidth(float value);

		// BUFFERS //
		// VAO
		GLuint generateVAO();
		void deleteVAO(GLuint& VAO);
		void bindVAO(GLuint VAO);
		void unbindVAO();

		// VBO
		void bindVBO(GLuint VBO);
		void bindVBOSubData(GLsizei Offset, GLsizei Size, GLvoid* Data);
		// VBO - Array
		void bindArray(GLuint VBO, GLsizeiptr length, GLvoid * data, GLenum usage);
		void setVertexAttrib(GLuint bufferIndex, int valueCount, DataType dataType = DataType::FLOAT, GLuint m_strideSize = 0, GLuint m_strideOffset = 0);
		void setVertexAttribInstancing(GLuint bufferIndex);
		// VBO - Index
		void bindVBOI(GLuint VBOI);
		void bindIndices(GLuint VBO, GLsizeiptr length, GLvoid * data, GLenum usage);

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