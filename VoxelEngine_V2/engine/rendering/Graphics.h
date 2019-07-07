// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <map>

namespace Vxl
{
	// Forward Declare
	class Vector2;
	class Vector3;
	class Vector4;
	class Color3F;
	class Color4F;
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;
	class Shader;

	// Vendor Types
	enum class glVendorType
	{
		UNKNOWN = 0,
		NVIDIA,
		AMD,
		INTEL
	};
	// Object Types
	enum class ObjectType
	{
		NONE = 0, // Error (Used for placeholder)

		BUFFER,
		SHADER,
		PROGRAM,
		VERTEX_ARRAY,
		QUERY,
		PROGRAM_PIPELINE,
		TRANSFORM_FEEDBACK,
		SAMPLER,
		TEXTURE,
		RENDERBUFFER,
		FRAMEBUFFER
	};
	// Shader Types
	enum class ShaderType
	{
		NONE = 0, // Error (Used for placeholder)

		VERTEX,
		FRAGMENT,
		GEOMETRY,
		TESSELATION_CONTROL,
		TESSELATION_EVALUATION ,
		COMPUTE,
	};

	// Special Containers //
	struct UniformData
	{
		uint32_t location;
		uint32_t type;
		std::string name;
	};

	// ~ Object Typedefs ~ //
	typedef uint32_t ShaderID;
	typedef uint32_t ShaderProgramID;

	// Graphics Caller
	namespace Graphics
	{
		// Static information
		extern int GLVersionMajor;
		extern int GLVersionMinor;
		extern int GLObjectNameMaxLength;
		extern float GLMaxAnisotropy;

		extern std::string Gpu_Renderer;
		extern std::string Gpu_OpenGLVersion;
		extern std::string Gpu_Vendor;
		extern glVendorType Vendor;

		// ~ Setup ~ //
		bool Setup();

		// ~ GPU Name ~ //
		void SetGLName(ObjectType identifier, uint32_t id, const std::string &label);

		// ~ Uniforms ~ //
		struct Uniform
		{
			uint32_t location;
			uint32_t type;
			int size;
			std::string name;

			// Send Uniform to Shader
			template<typename Type>
			void Send(Type data)
			{
				VXL_ASSERT(false, "Uniform Type does not exist");
			}
			// Send Uniform to Shader [Matrix only Version]
			template<typename Type>
			void SendMatrix(Type data, bool transposeMatrix)
			{
				VXL_ASSERT(false, "Uniform Type does not exist");
			}

			// [SLOWER BUT CAN BE CALLED OUTSIDE PROGRAM BINDING]
			// Send Uniform to Shader
			template<typename Type>
			void Send(ShaderProgramID id, Type data)
			{
				VXL_ASSERT(false, "Uniform Type does not exist");
			}
			// [SLOWER BUT CAN BE CALLED OUTSIDE PROGRAM BINDING]
			// Send Uniform to Shader [Matrix only Version]
			template<typename Type>
			void SendMatrix(ShaderProgramID id, Type data, bool transposeMatrix)
			{
				VXL_ASSERT(false, "Uniform Type does not exist");
			}
		};
		// Uniform Template Specialization //
		template<>void Uniform::Send<bool>    (bool data);
		template<>void Uniform::Send<int>     (int data);
		template<>void Uniform::Send<uint32_t>(uint32_t data);
		template<>void Uniform::Send<float>   (float data);
		template<>void Uniform::Send<double>  (double data);
		template<>void Uniform::Send<Vector2&>(Vector2& data);
		template<>void Uniform::Send<Vector2> (Vector2 data);
		template<>void Uniform::Send<Vector3&>(Vector3& data);
		template<>void Uniform::Send<Vector3> (Vector3 data);
		template<>void Uniform::Send<Vector4&>(Vector4& data);
		template<>void Uniform::Send<Vector4> (Vector4 data);
		template<>void Uniform::Send<Color3F&>(Color3F& data);
		template<>void Uniform::Send<Color3F> (Color3F data);
		template<>void Uniform::Send<Color4F&>(Color4F& data);
		template<>void Uniform::Send<Color4F> (Color4F data);

		template<>void Uniform::SendMatrix<Matrix2x2&>(Matrix2x2& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix2x2> (Matrix2x2 data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix3x3&>(Matrix3x3& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix3x3> (Matrix3x3 data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix4x4&>(Matrix4x4& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix4x4> (Matrix4x4 data, bool transposeMatrix);

		template<>void Uniform::Send<bool>    (ShaderProgramID id, bool data);
		template<>void Uniform::Send<int>     (ShaderProgramID id, int data);
		template<>void Uniform::Send<uint32_t>(ShaderProgramID id, uint32_t data);
		template<>void Uniform::Send<float>   (ShaderProgramID id, float data);
		template<>void Uniform::Send<double>  (ShaderProgramID id, double data);
		template<>void Uniform::Send<Vector2&>(ShaderProgramID id, Vector2& data);
		template<>void Uniform::Send<Vector2> (ShaderProgramID id, Vector2 data);
		template<>void Uniform::Send<Vector3&>(ShaderProgramID id, Vector3& data);
		template<>void Uniform::Send<Vector3> (ShaderProgramID id, Vector3 data);
		template<>void Uniform::Send<Vector4&>(ShaderProgramID id, Vector4& data);
		template<>void Uniform::Send<Vector4> (ShaderProgramID id, Vector4 data);
		template<>void Uniform::Send<Color3F&>(ShaderProgramID id, Color3F& data);
		template<>void Uniform::Send<Color3F> (ShaderProgramID id, Color3F data);
		template<>void Uniform::Send<Color4F&>(ShaderProgramID id, Color4F& data);
		template<>void Uniform::Send<Color4F> (ShaderProgramID id, Color4F data);

		template<>void Uniform::SendMatrix<Matrix2x2&>(ShaderProgramID id, Matrix2x2& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix2x2> (ShaderProgramID id, Matrix2x2 data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix3x3&>(ShaderProgramID id, Matrix3x3& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix3x3> (ShaderProgramID id, Matrix3x3 data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix4x4&>(ShaderProgramID id, Matrix4x4& data, bool transposeMatrix);
		template<>void Uniform::SendMatrix<Matrix4x4> (ShaderProgramID id, Matrix4x4 data, bool transposeMatrix);

		struct UniformBlock
		{
			uint32_t blockIndex;
			uint32_t bindingPoint;
			int size;
			std::string name;
		};

		struct UniformSubroutine
		{
			uint32_t shaderType;
			std::vector<uint32_t> indices;
			uint32_t indexCount;
			std::map<std::string, uint32_t> uniforms;
			std::map<std::string, uint32_t> functions;

			void Connect(const std::string& UniformName, const std::string& FunctionName);
			void Bind() const;
		};

		// ~ Shader ~ //
		namespace Shader
		{
			ShaderID	Create(ShaderType shaderType);
			void		Delete(ShaderID id);
			bool		Compile(ShaderID id, ShaderType shaderType, const char* source);
			std::string	GetError(ShaderID id);
			std::string GetName(ShaderType shaderType);
		}

		// ~ Shader Program ~ //
		namespace ShaderProgram
		{
			ShaderProgramID Create(void);
			void			Delete(ShaderProgramID program);
			void			AttachShader(ShaderProgramID program, ShaderID id);
			void			DetachShader(ShaderProgramID program, ShaderID id);
			bool			Link(ShaderProgramID program);
			bool			Validate(ShaderProgramID program);
			void			Enable(ShaderProgramID program);
			void			Disable();
			std::string		GetError(ShaderProgramID id);

			std::map<std::string, Uniform> AcquireUniforms(ShaderProgramID id);
			std::map<std::string, UniformBlock> AcquireUniformBlocks(ShaderProgramID id);
			std::map<ShaderType, UniformSubroutine> AcquireUniformSubroutines(ShaderProgramID id, std::vector<Vxl::Shader*> shaders);
		}

		

		// ~ Uniforms ~ //
		//static std::vector<UniformData> GetUniforms(ShaderProgramID id);
	};

}