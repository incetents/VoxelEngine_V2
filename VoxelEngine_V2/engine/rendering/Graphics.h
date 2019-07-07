// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <map>

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

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

	// ~ Vendor Types ~ //
	enum class glVendorType
	{
		UNKNOWN = 0,
		NVIDIA,
		AMD,
		INTEL
	};
	// ~ Object Types ~ //
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
	// ~ Shader Types ~ //
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
	// ~ Attribute Type ~ //
	enum class AttributeType
	{
		NONE = -1, // Error (Used for placeholder)

		INT,
		UINT,
		FLOAT,
		DOUBLE,
		VEC2,
		VEC3,
		VEC4,
		MAT2,
		MAT3,
		MAT4
	};
	// ~ Buffer Bit Type ~ //
	enum class BufferBit
	{
		NONE = 0, // Error (Used for placeholder) [0 to prevent blit error]

		COLOR,
		DEPTH,
		STENCIL
	};
	// ~ Cull Modes ~ //
	enum class CullMode
	{
		NONE = 0, // Error (Used for placeholder)

		NO_CULL,
		COUNTER_CLOCKWISE,
		CLOCKWISE
	};
	// ~ Blend Source ~ //
	enum class BlendSource
	{
		NONE = 0, // Error (Used for placeholder)

		ZERO,
		ONE,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA
	};
	// ~ Blend Destination ~ //
	enum class BlendDestination
	{
		NONE = 0, // Error (Used for placeholder)

		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA
	};
	// ~ Blend Equation ~ //
	enum class BlendEquation
	{
		NONE = 0, // Error (Used for placeholder)

		FUNC_ADD,
		FUNC_SUBTRACT,
		FUNC_REVERSE_SUBTRACT,
		MIN,
		MAX
	};
	// ~ Depth Pass Rule ~ //
	enum class DepthPassRule
	{
		NONE = 0, // Error (Used for placeholder)

		LESS,
		EQUAL,
		GREATER,
		LESS_OR_EQUAL,
		GREATER_OR_EQUAL,
		NOT_EQUAL,
		ALWAYS
	};
	// ~ Data Types ~ //
	enum class DataType
	{
		NONE = 0, // Error (Used for placeholder)

		BYTE,
		UNSIGNED_BYTE,
		SHORT,
		UNSIGNED_SHORT,
		INT,
		UNSIGNED_INT,
		HALF_FLOAT,
		FLOAT,
		DOUBLE,
		FIXED,
		INT_2_10_10_10_REV,
		UNSIGNED_INT_2_10_10_10_REV,
		UNSIGNED_INT_10F_11F_11F_REV
	};
	// ~ Buffer Usage ~ //
	enum class BufferUsage
	{
		NONE = 0, // Error (Used for placeholder)

		STREAM_DRAW, // The data store contents will be modified once and used at most a few times.
		STREAM_READ,
		STREAM_COPY,
		STATIC_DRAW, // The data store contents will be modified once and used many times.
		STATIC_READ,
		STATIC_COPY,
		DYNAMIC_DRAW, // The data store contents will be modified repeatedly and used many times.
		DYNAMIC_READ,
		DYNAMIC_COPY
	};
	// ~ Draw Type ~ //
	enum class DrawType
	{
		NONE = 0, // Error (Used for placeholder)

		POINTS,						// Geom / Frag	(points)
		LINES,						// Geom / Frag	(lines)
		LINE_STRIP,					// Geom / Frag	(lines)
		LINE_LOOP,					// Geom / Frag	(lines)
		LINES_ADJACENCY,			// Geom			(lines adjacency)
		LINE_STRIP_ADJACENCY,		// Geom			(lines adjacency)
		TRIANGLES,					// Geom / Frag	(triangles)
		TRIANGLE_STRIP,				// Geom / Frag	(triangles)
		TRIANGLE_FAN,				// Geom / Frag	(triangles)
		TRIANGLES_ADJACENCY,		// Geom			(triangles adjacency)
		TRIANGLE_STRIP_ADJACENCY,	// Geom (triangles adjacency)
		PATCHES						// Tesselation Control
	};
	// ~ Draw SubType ~ //
	enum class DrawSubType
	{
		NONE = -1,
			
		POINTS,
		LINES,
		TRIANGLES
	};
	// ~ Draw Mode ~ //
	enum class DrawMode
	{
		NONE = -1, // Error (Used for placeholder)

		ARRAY,
		ARRAY_INSTANCED,
		INDEXED,
		INDEXED_INSTANCED
	};

	// ~ Object Typedefs ~ //
	typedef uint32_t ShaderID;
	typedef uint32_t ShaderProgramID;
	typedef uint32_t VAOID;
	typedef uint32_t VBOID;
	typedef uint32_t EBOID;

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
		void initHints();

		// ~ GPU Name ~ //
		void SetGLName(ObjectType identifier, uint32_t id, const std::string &label);

		// ~ States ~ //
		void SetCullMode(CullMode cullmode);
		void SetBlendState(bool state);
		void SetDepthTestState(bool state);
		void SetWireframeState(bool state);
		void SetDepthMask(bool state);
		void SetSeamlessCubemap(bool state);

		// ~ Blending ~ //
		void SetBlendMode(BlendSource src, BlendDestination dest);
		void SetBlendEquation(BlendEquation eq);

		// ~ Depth ~ //
		void SetDepthPassRule(DepthPassRule rule);

		// ~ Viewport ~ //
		void SetViewport(int x, int y, int w, int h);

		// ~ Clear Buffer [Setup] ~ //
		void SetClearColor(float r, float g, float b, float a);
		void SetClearColor(const Color3F& c, float a);
		void SetClearColor(const Color4F& c);
		void SetClearDepth(float f);
		void SetClearStencil(int i);
		// ~ Clear Buffer [Call] ~ //
		void ClearBuffer(BufferBit clearBit);
		void ClearBuffer(BufferBit clearBit, BufferBit clearBit2);
		void ClearAllBuffers();

		// ~ Conversion ~ //
		uint32_t GetValueCount(AttributeType type);
		uint32_t GetSize(AttributeType type);
		DataType GetDataType(AttributeType type);
		DrawSubType GetDrawSubType(DrawType type);

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

		// ~ Buffers ~ //
		namespace VAO
		{
			VAOID	Create(void);
			void	Delete(VAOID id);
			void	Bind(VAOID id);
			void	Unbind();
		}
		namespace VBO
		{
			VBOID	Create(void);
			void	Delete(VBOID id);
			void	Bind(VBOID id);
			void	Unbind(void);
			void	BindData(__int64 length, void* data, BufferUsage usage);
			void	BindSubData(int OffsetBytes, int SizeBytes, void* data);

			void	SetVertexAttribState(uint32_t bufferIndex, bool state);
			void	SetVertexAttrib(uint32_t bufferIndex, int valueCount, DataType datatype, uint32_t strideSize, uint32_t strideOffset, bool normalized);
			void	SetVertexAttribDivisor(uint32_t bufferIndex, uint32_t divisor);
		}

		namespace EBO
		{
			EBOID	Create(void);
			void	Delete(EBOID id);
			void	Bind(EBOID id);
			void	Unbind(void);
			void	BindData(__int64 length, void* data, BufferUsage usage);
			void	BindSubData(int OffsetBytes, int SizeBytes, void* data);
		}

		// ~ Drawing ~ //
		namespace Draw
		{
			void Array(DrawType type, uint32_t count);
			void Indexed(DrawType type, uint32_t count);
			void ArrayInstanced(DrawType type, uint32_t count, uint32_t instanceCount);
			void IndexedInstanced(DrawType type, uint32_t count, uint32_t instanceCount);
		}
	};

}