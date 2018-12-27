// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "glUtil.h"

#include "../utilities/logger.h"
#include "../Math/Color.h"
#include "../math/Vector4.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>

namespace Vxl
{
	bool glUtil::initGlew()
	{
		if (gl3wInit())
		{
			Logger.error("failed to initialize OpenGL");
			std::system("pause");
			return false;
		}
		if (!gl3wIsSupported(3, 2))
		{
			Logger.error("OpenGL 3.2 not supported");
			std::system("pause");
			return false;
		}

		return true;
	}

	// Init GL Hints
	void glUtil::initHints()
	{
		// Set Default Cull Mode
		cullMode(Cull_Type::COUNTER_CLOCKWISE);
		// Set Default Blend Mode
		blendMode(Blend_Source::SRC_ALPHA, Blend_Destination::ONE_MINUS_SRC_ALPHA);
		// Set Default Depth Test
		depthTest(Depth_Pass_Rule::LESS);
		// Set Default Wireframe Mode
		wireframe(false);

		// Cubemap edges are hidden
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void glUtil::CheckVersion()
	{
		// Get version info
		Logger.log("Renderer: " + std::string(glUtil::getRendererVersion()));
		Logger.log("OpenGL version supported: " + std::string(glUtil::getOpenGLVersion()));
		Logger.log("~~~");
	}

	const char* glUtil::getRendererVersion()
	{
		return reinterpret_cast<char const*>(glGetString(GL_RENDERER)); // get renderer string (GLubyte*)
	}
	const char* glUtil::getOpenGLVersion()
	{
		return reinterpret_cast<char const*>(glGetString(GL_VERSION)); // version as a string (GLubyte*)
	}

	// Get Format Type from channel count
	Format_Type glUtil::getFormatFloat(int ChannelCount)
	{
		switch (ChannelCount)
		{
		default:
		case 0:
			return Format_Type::NONE;
		case 1:
			return Format_Type::GRAYSCALE;
		case 2:
			return Format_Type::RG;
		case 3:
			return Format_Type::RGB;
		case 4:
			return Format_Type::RGBA;
		}
	}
	Format_Type glUtil::getFormatInt(int ChannelCount)
	{
		switch (ChannelCount)
		{
		default:
		case 0:
			return Format_Type::NONE;
		case 1:
			return Format_Type::GRAYSCALE_INT;
		case 2:
			return Format_Type::RG_INT;
		case 3:
			return Format_Type::RGB_INT;
		case 4:
			return Format_Type::RGBA_INT;
		}
	}

	void glUtil::clearColor()
	{
		clearColor(0.f, 0.f, 0.f, 0.f);
	}
	void glUtil::clearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	void glUtil::clearColor(int r, int g, int b, int a)
	{
		glClearColor(
			static_cast<float>(r) / 255.0f,
			static_cast<float>(g) / 255.0f,
			static_cast<float>(b) / 255.0f,
			static_cast<float>(a) / 255.0f);
	}
	void glUtil::clearColor(const Color3F& c)
	{
		clearColor(c.r, c.g, c.b, 1.0f);
	}
	void glUtil::clearColor(const Color4F& c)
	{
		clearColor(c.r, c.g, c.b, c.a);
	}

	void glUtil::clearBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	void glUtil::clearBuffer(Buffer_Bit_Type a)
	{
		glClear(GLbitfield(a));
	}
	void glUtil::clearBuffer(Buffer_Bit_Type a, Buffer_Bit_Type b)
	{
		glClear(GLbitfield(a) | GLbitfield(b));
	}

	void glUtil::clearDepth(float f)
	{
		assert(f >= 0.0f && f <= 0.0f);
		glClearDepth(f);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void glUtil::clearStencil(int f)
	{
		assert(f >= 0.0f && f <= 0.0f);
		glClearStencil(f);
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void glUtil::cullMode(Cull_Type cull)
	{
		switch (cull)
		{
		case Cull_Type::NONE:
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
		}
	}

	// Blend Mode (Calculates transparency in shaders)
	void glUtil::blendMode(Blend_Source src = Blend_Source::SRC_ALPHA, Blend_Destination dst = Blend_Destination::ONE_MINUS_SRC_ALPHA)
	{
		glEnable(GL_BLEND);
		glBlendFunc((GLenum)src, (GLenum)dst);
	}

	// Depth Test (What depth value will overwrite the existing one)
	void glUtil::depthTest(Depth_Pass_Rule Rule = Depth_Pass_Rule::LESS_OR_EQUAL)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc((GLenum)Rule);
	}

	// Wireframe Mode
	void glUtil::wireframe(bool state)
	{
		if (state)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Wireframe Line Width
	void glUtil::wireframeWidth(float value)
	{
		glLineWidth(value);
	}

	// BUFFERS //
	void glUtil::generateVAO(GLuint& VAO, GLuint& VBO_Start, const GLuint& TotalVBOs)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(TotalVBOs, &VBO_Start);
	}
	void glUtil::deleteVAO(GLuint& VAO, GLuint& VBO_Start, const GLuint& TotalVBOs)
	{
		glDeleteBuffers(TotalVBOs, &VBO_Start);
		glDeleteVertexArrays(1, &VAO);
	}
	void glUtil::bindVAO(GLuint& VAO)
	{
		glBindVertexArray(VAO);
	}
	void glUtil::unbindVAO()
	{
		glBindVertexArray(0);
	}

	void glUtil::bindArray(GLuint VBO, GLsizeiptr length, GLvoid* data, GLenum usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, length, data, usage);
	}
	void glUtil::setVertexAttrib(GLuint bufferIndex, int valueCount, DataType dataType)
	{
		glEnableVertexAttribArray(bufferIndex);
		glVertexAttribPointer(bufferIndex, valueCount, (GLenum)dataType, GL_FALSE, 0, 0);
	}
	void glUtil::setVertexAttribInstancing(GLuint bufferIndex)
	{
		glEnableVertexAttribArray(bufferIndex + 0);
		glVertexAttribPointer(bufferIndex + 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)0);
		glEnableVertexAttribArray(bufferIndex + 1);
		glVertexAttribPointer(bufferIndex + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(sizeof(Vector4)));
		glEnableVertexAttribArray(bufferIndex + 2);
		glVertexAttribPointer(bufferIndex + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(2 * sizeof(Vector4)));
		glEnableVertexAttribArray(bufferIndex + 3);
		glVertexAttribPointer(bufferIndex + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(3 * sizeof(Vector4)));

		glVertexAttribDivisor(bufferIndex + 0, 1);
		glVertexAttribDivisor(bufferIndex + 1, 1);
		glVertexAttribDivisor(bufferIndex + 2, 1);
		glVertexAttribDivisor(bufferIndex + 3, 1);
	}

	void glUtil::bindIndices(GLuint VBO, GLsizeiptr length, GLvoid* data, GLenum usage)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
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
	void glUtil::setActiveTexture(Active_Texture level)
	{
		glActiveTexture(GLenum(level));
	}
	void glUtil::setActiveTexture(int level)
	{
		glActiveTexture(GL_TEXTURE0 + level);
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
		GLint MaxBindingPoints;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MaxBindingPoints);
		return MaxBindingPoints;
	}
	GLint glUtil::GetMaxFBOColorAttachments()
	{
		GLint maxAttach = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
		return maxAttach;
	}
}
