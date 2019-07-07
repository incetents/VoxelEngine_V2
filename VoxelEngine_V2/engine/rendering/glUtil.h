// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"
#include "../utilities/singleton.h"

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
		AMD,
		INTEL
	};

	static class glInfo : public Singleton<class glInfo>
	{
	public:
		void Setup();

		// OpenGL Version
		int GLVersionMajor = 0;
		int GLVersionMinor = 0;
		// Vendor Type
		VendorType Vendor = VendorType::UNKNOWN;
		// Graphics Info
		std::string Gpu_Renderer;
		std::string Gpu_OpenGLVersion;
		std::string Gpu_Vendor;

		// Variables
		float MAX_ANISOTROPY = -1;

	} SingletonInstance(glInfo);

	namespace glUtil
	{
		// Setup Glew for OpenGL
		bool initGlew();

		// Init hints
		//void initHints();

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

		// Check if string name matches GL type
		bool VeryifyDataType(const std::string& name, GLenum type);

		// Set OpenGL Name
		//	static int GLNameMaxLength = -1;
		//	void setGLName(glNameType identifier, GLuint name, const std::string &label);

		// Get Format Type from channel count
		Channel_Type getChannelType(int ChannelCount);
		unsigned int getChannelCount(Channel_Type type);
		unsigned int getChannelCount(Format_Type format);

		//	// Get info from ShaderDataType
		//	uint32_t getValueCount(ShaderDataType data);
		//	uint32_t getSize(ShaderDataType data);
		//	DataType getDataType(ShaderDataType data);

		// Get Pixel/Channel Data From DepthFormat
		void getPixelChannelData(DepthFormat_Type format, Channel_Type& channelType, Pixel_Type& pixelType);

		//	// Clear Setup
		//	void clearColor(float r, float g, float b, float a = 1.0f);
		//	void clearColor(const Color3F& c, float a = 1.0f);
		//	void clearColor(const Color4F& c);
		//	void clearDepth(float);
		//	void clearStencil(int);
		//	// Clear Buffer
		//	void clearBuffer();
		//	void clearBuffer(UINT bitCode);
		//	void clearBuffer(Buffer_Bit_Type a, Buffer_Bit_Type b = Buffer_Bit_Type::NONE, Buffer_Bit_Type c = Buffer_Bit_Type::NONE, Buffer_Bit_Type d = Buffer_Bit_Type::NONE);
		//	
		//	// Cull Mode
		//	void cullMode(Cull_Type);
		//	
		//	// Blend Data
		//	void blendState(bool state);
		//	void blendMode(Blend_Source src, Blend_Destination dst);
		//	void blendEquation(Blend_Equation equation);
		//
		//	// Depth Rule
		//	void depthState(bool state);
		//	void depthMask(bool state);
		//	void depthTest(Depth_Pass_Rule Rule);
		//	
		//	// Wireframe
		//	void wireframe(bool state);
		//	
		//	// viewport
		//	void viewport(GLsizei x, GLsizei y, GLsizei w, GLsizei h);

		//	// BUFFERS //
		//	// VAO
		//	GLuint generateVAO();
		//	void deleteVAO(GLuint& VAO);
		//	void bindVAO(GLuint VAO);
		//	void unbindVAO();
		//	
		//	// VBO
		//	void bindVBO(GLuint VBO);
		//	void bindVBOSubData(GLuint VBO, GLsizei Offset, GLsizei Size, GLvoid* Data);
		//	// VBO - Array
		//	void bindArray(GLuint VBO, GLsizeiptr length, GLvoid * data, GLenum usage);
		//	void setVertexAttrib(GLuint bufferIndex, int valueCount, DataType dataType = DataType::FLOAT, GLuint strideSize = 0, GLuint strideOffset = 0, bool normalized = false);
		//	void setVertexAttribDivisor(GLuint bufferIndex, GLuint divisor);
		//	//void setVertexAttribInstancing(GLuint bufferIndex);
		//	// EBO - Index
		//	void bindEBO(GLuint EBO);
		//	void bindIndices(GLuint EBO, GLsizeiptr length, GLvoid * data, GLenum usage);
		//	
		//	// Draw Buffer (VAO needs to be bound before this call)
		//	void drawArray(Draw_Type type, GLuint count);
		//	void drawArrayIndexed(Draw_Type type, GLuint count);
		//	
		//	// Draw Type
		//	bool isDrawTypeTriangles(Draw_Type type);
		//	bool isDrawTypeLines(Draw_Type type);
		//	bool isDrawTypePoints(Draw_Type type);

		// TEXTURES //
		void setActiveTexture(Active_Texture level);
		void setActiveTexture(int level);
		void setActiveTexture(unsigned int level);

		void bindTexture(Texture_Type type, GLuint& texture_id);
		void unbindTexture(Texture_Type type);

		// MISC //
		//std::string shaderTypeToString(const ShaderType& type);

		// GET OPENGL INFO //
		GLint GetMaxUniformBufferBindings();
		GLint GetMaxFBOColorAttachments();
	}
}