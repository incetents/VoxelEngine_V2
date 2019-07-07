// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Graphics.h"

#include <GL/gl3w.h>

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

#include "Shader.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>

namespace Vxl
{
	// Statics
	int Graphics::GLVersionMajor = -1;
	int Graphics::GLVersionMinor = -1;
	int Graphics::GLObjectNameMaxLength = -1;
	float Graphics::GLMaxAnisotropy = -1;

	std::string Graphics::Gpu_Renderer;
	std::string Graphics::Gpu_OpenGLVersion;
	std::string Graphics::Gpu_Vendor;
	glVendorType Graphics::Vendor = glVendorType::UNKNOWN;

	// ~ Graphics Enums ~ //
	int GL_ObjectType[] =
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
	int GL_ShaderType[]
	{
		-1, // Error (Used for placeholder)

		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER,
		GL_TESS_CONTROL_SHADER,
		GL_TESS_EVALUATION_SHADER,
		GL_COMPUTE_SHADER
	};
	const char* GL_ShaderNames[]
	{
		"NULL", // Error (Used for placeholder)

		"VERTEX_SHADER",
		"FRAGMENT_SHADER",
		"GEOMETRY_SHADER",
		"TESS_CONTROL_SHADER",
		"TESS_EVALUATION_SHADER",
		"COMPUTE_SHADER"
	};

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
		auto test = glGetString(GL_RENDERER);

		Gpu_Renderer = std::string((reinterpret_cast<char const*>(glGetString(GL_RENDERER))));
		Gpu_OpenGLVersion = std::string((reinterpret_cast<char const*>(glGetString(GL_VERSION))));
		Gpu_Vendor = std::string((reinterpret_cast<char const*>(glGetString(GL_VENDOR))));

		// Acquire Vendor
		std::string VendorStr = Gpu_Vendor;
		std::transform(VendorStr.begin(), VendorStr.end(), VendorStr.begin(), ::tolower);
		if (VendorStr.find("nvidia") != -1)
			Vendor = glVendorType::NVIDIA;
		else if (VendorStr.find("ati") != -1 || VendorStr.find("radeon") != -1)
			Vendor = glVendorType::AMD;
		else if (VendorStr.find("intel") != -1)
			Vendor = glVendorType::INTEL;

		// Acquire Restrictions
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &GLMaxAnisotropy);

		// All good
		return true;
	}

	// ~ GPU Name ~ //
	void Graphics::SetGLName(ObjectType identifier, uint32_t id, const std::string &label)
	{
#if defined(GLOBAL_USE_GLNAMES) && !defined(GLOBAL_ERROR_CALLBACK)
		//
		if (GLObjectNameMaxLength == -1)
			glGetIntegerv(GL_MAX_LABEL_LENGTH, &GLObjectNameMaxLength);

		std::string labelEdit = label + " (" + std::to_string(id) + ")";
		VXL_ASSERT(labelEdit.size() < GLObjectNameMaxLength, "GLName is too big");
		glObjectLabel((GLenum)identifier, id, static_cast<GLsizei>(labelEdit.size()), labelEdit.c_str());
		//
#endif
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
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix2x2&>(Matrix2x2& data, bool transposeMatrix)
	{
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3>(Matrix3x3 data, bool transposeMatrix)
	{
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
	}
	template<>
	void Graphics::Uniform::SendMatrix<Matrix3x3&>(Matrix3x3& data, bool transposeMatrix)
	{
		glUniformMatrix4fv(location, 1, transposeMatrix, data.GetStartPointer());
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

		if (id == -1)
			Logger.error("GL ERROR: -1 glCreateShader()");

		return id;
	}
	void Graphics::Shader::Delete(ShaderID id)
	{
		if (id == -1)
			Logger.error("GL ERROR: -1 glDeleteShader()");
		else
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

		if (program == -1)
			Logger.error("GL ERROR: -1 glCreateProgram()");

		return program;
	}
	void Graphics::ShaderProgram::Delete(ShaderProgramID program)
	{
		if (program == -1)
			Logger.error("GL ERROR: -1 glDeleteShader()");
		else
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
		glUseProgram(program);
	}
	void Graphics::ShaderProgram::Disable()
	{
		glUseProgram(0);
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
		GLint uniform_count;

		const GLsizei bufSize = 128; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

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

		uint32_t shaderCount = shaders.size();
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
}