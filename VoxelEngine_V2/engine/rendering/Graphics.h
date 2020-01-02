// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <map>

#define BUFFER_OFFSET(i) ((char *)NULL + (i)) 

// Target OPENGL Version for context
#define OPENGL_MAJOR 4
#define OPENGL_MINOR 2

namespace Vxl
{
	// Forward Declare
	class Color3F;
	class Color4F;
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;
	class Texture2D;
	class RenderTexture;
	class RenderBuffer;
	class BaseTexture;

	template<typename Type>
	class RawArray;

	template<typename Type>
	class _Vector2;

	template<typename Type>
	class _Vector3;

	template<typename Type>
	class _Vector4;

	// ~ Vendor Types ~ //
	enum class VendorType
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
	// ~ Blending ~ //
	struct BlendFunction
	{
		BlendSource			source		= BlendSource::SRC_ALPHA;
		BlendDestination	destination	= BlendDestination::ONE_MINUS_SRC_ALPHA;
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
	// ~ Uniform Type ~~ //
	enum class UniformType
	{
		NONE = 0, // Error (Used for placeholder)

		// Data
		FLOAT,
		FLOAT_VEC2,
		FLOAT_VEC3,
		FLOAT_VEC4,
		DOUBLE,
		DOUBLE_VEC2,
		DOUBLE_VEC3,
		DOUBLE_VEC4,
		INT,
		INT_VEC2,
		INT_VEC3,
		INT_VEC4,
		UNSIGNED_INT,
		UNSIGNED_INT_VEC2,
		UNSIGNED_INT_VEC3,
		UNSIGNED_INT_VEC4,
		BOOL,
		BOOL_VEC2,
		BOOL_VEC3,
		BOOL_VEC4,
		FLOAT_MAT2,
		FLOAT_MAT3,
		FLOAT_MAT4,
		FLOAT_MAT2x3,
		FLOAT_MAT2x4,
		FLOAT_MAT3x2,
		FLOAT_MAT3x4,
		FLOAT_MAT4x2,
		FLOAT_MAT4x3,
		DOUBLE_MAT2,
		DOUBLE_MAT3,
		DOUBLE_MAT4,
		DOUBLE_MAT2x3,
		DOUBLE_MAT2x4,
		DOUBLE_MAT3x2,
		DOUBLE_MAT3x4,
		DOUBLE_MAT4x2,
		DOUBLE_MAT4x3,

		// non-regular data from this point on
		__DATA_TYPE_CUTOFF__,

		SAMPLER_1D,
		SAMPLER_2D,
		SAMPLER_3D,
		SAMPLER_CUBE,
		SAMPLER_1D_SHADOW,
		SAMPLER_2D_SHADOW,
		SAMPLER_1D_ARRAY,
		SAMPLER_2D_ARRAY,
		SAMPLER_1D_ARRAY_SHADOW,
		SAMPLER_2D_ARRAY_SHADOW,
		SAMPLER_2D_MULTISAMPLE,
		SAMPLER_2D_MULTISAMPLE_ARRAY,
		SAMPLER_CUBE_SHADOW,
		SAMPLER_BUFFER,
		SAMPLER_2D_RECT,
		SAMPLER_2D_RECT_SHADOW,
		INT_SAMPLER_1D,
		INT_SAMPLER_2D,
		INT_SAMPLER_3D,
		INT_SAMPLER_CUBE,
		INT_SAMPLER_1D_ARRAY,
		INT_SAMPLER_2D_ARRAY,
		INT_SAMPLER_2D_MULTISAMPLE,
		INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		INT_SAMPLER_BUFFER,
		INT_SAMPLER_2D_RECT, 
		UNSIGNED_INT_SAMPLER_1D,
		UNSIGNED_INT_SAMPLER_2D,
		UNSIGNED_INT_SAMPLER_3D,
		UNSIGNED_INT_SAMPLER_CUBE,
		UNSIGNED_INT_SAMPLER_1D_ARRAY,
		UNSIGNED_INT_SAMPLER_2D_ARRAY,
		UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
		UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		UNSIGNED_INT_SAMPLER_BUFFER,
		UNSIGNED_INT_SAMPLER_2D_RECT,
		IMAGE_1D,
		IMAGE_2D,
		IMAGE_3D,
		IMAGE_2D_RECT,
		IMAGE_CUBE,
		IMAGE_BUFFER,
		IMAGE_1D_ARRAY,
		IMAGE_2D_ARRAY,
		IMAGE_2D_MULTISAMPLE,
		IMAGE_2D_MULTISAMPLE_ARRAY,
		INT_IMAGE_1D,
		INT_IMAGE_2D,
		INT_IMAGE_3D,
		INT_IMAGE_2D_RECT,
		INT_IMAGE_CUBE,
		INT_IMAGE_BUFFER,
		INT_IMAGE_1D_ARRAY,
		INT_IMAGE_2D_ARRAY,
		INT_IMAGE_2D_MULTISAMPLE,
		INT_IMAGE_2D_MULTISAMPLE_ARRAY,
		UNSIGNED_INT_IMAGE_1D,
		UNSIGNED_INT_IMAGE_2D,
		UNSIGNED_INT_IMAGE_3D,
		UNSIGNED_INT_IMAGE_2D_RECT,
		UNSIGNED_INT_IMAGE_CUBE,
		UNSIGNED_INT_IMAGE_BUFFER,
		UNSIGNED_INT_IMAGE_1D_ARRAY,
		UNSIGNED_INT_IMAGE_2D_ARRAY,
		UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
		UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
		UNSIGNED_INT_ATOMIC_COUNTER
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
	// ~ Texture Info ~ //
	enum class TextureType
	{
		NONE = 0, // Error (Used for placeholder)

		TEX_1D,
		TEX_2D,
		TEX_3D,
		TEX_1D_ARRAY,
		TEX_2D_ARRAY,
		TEX_RECT,
		TEX_CUBEMAP,
		TEX_CUBEMAP_ARRAY,
		TEX_BUFFER,
		TEX_2D_MULTISAMPLE,
		TEX_2D_MULTISAMPLE_ARRAY,

		RENDERBUFFER // special case
	};
	enum class TextureWrapping
	{
		NONE = 0, // Error (Used for placeholder)

		REPEAT,
		CLAMP_BORDER,
		CLAMP_STRETCH,
		MIRROR_REPEAT,
		MIRROR_CLAMP
	};
	enum class TextureFilter
	{
		NONE = 0, // Error (Used for placeholder)

		NEAREST,
		LINEAR
	};
	enum class TextureFormat
	{
		NONE = 0, // Error (Used for placeholder)

		// 1 channel
		R8,
		R8_SNORM,
		R16,
		R16_SNORM,
		R16F,
		// 2 channels
		RG8,
		RG8_SNORM,
		RG16,
		RG16_SNORM,
		RG16F,
		// 3 channels
		RGB8,
		RGB8_SNORM,
		RGB16,
		RGB16_SNORM,
		RGB16F,
		// 4 channels
		RGBA8,
		RGBA8_SNORM,
		RGBA16,
		RGBA16_SNORM,
		RGBA16F,
		R11F_G11F_B10F,// GOOD FOR GBUFFER

		// [ Special ]
		SRGB8, // 3 channels
		SRGBA8, // 4 channels

		// [DepthFormat]
		STENCIL8,
		DEPTH16,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		DEPTH24_STENCIL8,
		DEPTH32F_STENCIL8
	};
	enum class TextureDepthFormat
	{
		NONE = 0, // Error (Used for placeholder)

		STENCIL8,
		DEPTH16,
		DEPTH24,
		DEPTH32,
		DEPTH32F,
		DEPTH24_STENCIL8,
		DEPTH32F_STENCIL8
	};
	enum class TextureChannelType
	{
		NONE = 0, // Error (Used for placeholder)

		R,		// 1 channel
		RG,		// 2 channels
		RGB,	// 3 channels
		RGBA,	// 4 channels

		BGR,	// 3 channels
		BGRA,	// 4 channels

		STENCIL,		// channel count varies 
		DEPTH,			// channel count varies
		DEPTH_STENCIL,	// channel count varies
	};
	enum class TexturePixelType
	{
		NONE = 0, // Error (Used for placeholder)

		UNSIGNED_BYTE,
		BYTE,
		UNSIGNED_SHORT,
		SHORT,
		UNSIGNED_INT,
		INT,
		HALF_FLOAT,
		FLOAT,
		UNSIGNED_BYTE_3_3_2,
		UNSIGNED_SHORT_5_6_5,
		UNSIGNED_SHORT_4_4_4_4,
		UNSIGNED_SHORT_5_5_5_1,
		UNSIGNED_INT_8_8_8_8,
		UNSIGNED_INT_10_10_10_2
	};
	enum class TextureLevel
	{
		NONE = 0, // Error (Used for placeholder)

		LEVEL0,
		LEVEL1,
		LEVEL2,
		LEVEL3,
		LEVEL4,
		LEVEL5,
		LEVEL6,
		LEVEL7,
		LEVEL8,

		TOTAL
	};
	enum class AnisotropicMode
	{
		NONE = 1,
		LOW = 4,
		MEDIUM = 8,
		HIGH = 16
	};
	enum class PixelAlignment
	{
		NONE = 0,
		ALIGN_1 = 1,
		ALIGN_2 = 2,
		ALIGN_4 = 4,
		ALIGN_8 = 8
	};

	enum class CubemapFace
	{
		NONE = 0,
		POSITIVE_X,
		NEGATIVE_X,
		POSITIVE_Y,
		NEGATIVE_Y,
		POSITIVE_Z,
		NEGATIVE_Z
	};

	// ~ Object Typedefs ~ //
	typedef uint32_t ShaderID;
	typedef uint32_t ShaderProgramID;
	typedef uint32_t VAOID;
	typedef uint32_t VBOID;
	typedef uint32_t EBOID;
	typedef uint32_t TextureID;
	typedef uint32_t RenderBufferID;
	typedef uint32_t FramebufferObjectID;
	typedef uint32_t UBOID;
	typedef uint32_t QueryID;

	// Graphics Caller
	namespace Graphics
	{
		// Shader Version
		extern const std::string GLSL_Version;

		// Static GPU information
		extern int GLVersionMajor;
		extern int GLVersionMinor;
		extern int GLObjectNameMaxLength;
		extern float GLMaxAnisotropy;
		extern int GLMaxFBOColorAttachments;
		extern int GLMaxUniformBindings;
		extern int GLMaxAttributes;

		extern std::string Gpu_Renderer;
		extern std::string Gpu_OpenGLVersion;
		extern std::string Gpu_Vendor;
		extern VendorType Vendor;

		extern int VRAM_Maximum_KB;
		extern int VRAM_Current_KB;

		// Static CPU information
		namespace CPU
		{
			namespace VirtualMemory
			{
				extern uint64_t Total_KB;
				extern uint64_t Used_KB;
				extern uint64_t UsedByProcess_KB;
			}
			namespace RAM
			{
				extern uint64_t Total_KB;
				extern uint64_t Used_KB;
				extern uint64_t UsedByProcess_KB;
			}
		}

		//extern int RAM;

		extern bool UsingErrorCallback;

		// ~ Setup ~ //
		bool Setup();
		void initHints();
		void InitOpenGLDebugCallback(void);

		// ~ SPECIAL ~ //
		void GetRuntimeGLValues(void);
		bool VeryifyDataType(const std::string& name, uint32_t type);

		// ~ GPU Name ~ //
		void SetGLName(ObjectType identifier, uint32_t id, const std::string &label);

		// ~ States ~ //
		void SetCullMode(CullMode cullmode);
		void SetBlendState(bool state);
		void SetDepthRead(bool state);
		void SetWireframeState(bool state);
		void SetDepthWrite(bool state);
		void SetSeamlessCubemap(bool state);
		void SetSRGBMode(bool state);

		// ~ State Getters ~ //
		bool GetDepthRead();

		// ~ Debug ~ // Expensive calls that are used for testing reasons
		namespace Debug
		{
			bool GetDepthRead();
		}

		// ~ Blending ~ //
		void SetBlendMode(BlendSource src, BlendDestination dest);
		void SetBlendMode(uint32_t attachmentIndex, BlendSource src, BlendDestination dest);
		void SetBlendEquation(BlendEquation eq);

		// ~ Depth ~ //
		void SetDepthPassRule(DepthPassRule rule);

		// ~ Viewport ~ //
		void SetViewport(int x, int y, int w, int h);

		// ~ LineWidth ~ //
		void SetLineWidth(float w);

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
		// Clears buffer of a specific fbo attachment //
		void ClearBufferFBOAttachment(uint32_t attachmentIndex, float r, float g, float b, float a);
		void ClearBufferFBOAttachment(uint32_t attachmentIndex, const Color3F& clearColor, float a);
		void ClearBufferFBOAttachment(uint32_t attachmentIndex, const Color4F& clearColor);

		// ~ Conversion ~ //
		uint32_t GetValueCount(AttributeType type);
		uint32_t GetSize(AttributeType type);
		DataType GetDataType(AttributeType type);
		DrawSubType GetDrawSubType(DrawType type);
		TexturePixelType GetPixelData(TextureDepthFormat format);
		TextureChannelType GetChannelType(int ChannelCount);
		TextureChannelType GetChannelType(TextureFormat format);
		uint32_t GetChannelCount(TextureChannelType type);
		uint32_t GetChannelCount(TextureFormat format);
		TextureFormat GetFormat(TextureDepthFormat format);

		// ~ Copy Texture ~ //
		//	void CopyTexture(
		//		TextureID src, TextureType srcType,
		//		TextureID dest, TextureType destType,
		//		uint32_t width, uint32_t height
		//	);

		// ~ Attributes ~ //
		struct Attribute
		{
			uint32_t location;
			uint32_t type;
			int size;
			std::string name;
		};

		// ~ Uniforms ~ //
		struct Uniform
		{
			uint32_t location = -1;
			uint32_t type;
			int size;
			std::string name;

			// Convenient Uniform Data
			UniformType uType;
			bool isData;

			// Send pure data
			void send(bool data) const;
			void send(int data) const;
			void send(uint32_t data) const;
			void send(float data) const;
			void send(double data) const;

			// Send vector data
			void send(const _Vector2<float>& data) const;
			void send(const _Vector3<float>& data) const;
			void send(const _Vector4<float>& data) const;
			void send(const Color3F& data) const;
			void send(const Color4F& data) const;

			void send(const _Vector2<double>& data) const;
			void send(const _Vector3<double>& data) const;
			void send(const _Vector4<double>& data) const;

			void send(const _Vector2<int>& data) const;
			void send(const _Vector3<int>& data) const;
			void send(const _Vector4<int>& data) const;

			void send(const _Vector2<uint32_t>& data) const;
			void send(const _Vector3<uint32_t>& data) const;
			void send(const _Vector4<uint32_t>& data) const;

			void send(const _Vector2<bool>& data) const;
			void send(const _Vector3<bool>& data) const;
			void send(const _Vector4<bool>& data) const;

			// Send matrix data
			void sendMatrix(const Matrix2x2& data, bool transpose) const;
			void sendMatrix(const Matrix3x3& data, bool transpose) const;
			void sendMatrix(const Matrix4x4& data, bool transpose) const;

			// Get Uniform from Shader [store data in CPU side instead, only used for getting default uniform values]
			void getFloat(ShaderID program, float& value) const;
			void getDouble(ShaderID program, double& value) const;
			void getInt(ShaderID program, int& value) const;
			void getUnsignedInt(ShaderID program, uint32_t& value) const;
			void getBool(ShaderID program, bool& value) const;
			
			void getVec2(ShaderID program, Vector2& value) const;
			void getVec2(ShaderID program, Vector2d& value) const;
			void getVec2(ShaderID program, Vector2i& value) const;
			void getVec2(ShaderID program, Vector2ui& value) const;
			void getVec2(ShaderID program, Vector2b& value) const;
			
			void getVec3(ShaderID program, Vector3& value) const;
			void getVec3(ShaderID program, Vector3d& value) const;
			void getVec3(ShaderID program, Vector3i& value) const;
			void getVec3(ShaderID program, Vector3ui& value) const;
			void getVec3(ShaderID program, Vector3b& value) const;
			
			void getVec4(ShaderID program, Vector4& value) const;
			void getVec4(ShaderID program, Vector4d& value) const;
			void getVec4(ShaderID program, Vector4i& value) const;
			void getVec4(ShaderID program, Vector4ui& value) const;
			void getVec4(ShaderID program, Vector4b& value) const;

			void getMat2(ShaderID program, Matrix2x2& value) const;
			void getMat3(ShaderID program, Matrix3x3& value) const;
			void getMat4(ShaderID program, Matrix4x4& value) const;
		};

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
			void bind() const;
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
			void			Disable(void);
			ShaderProgramID	GetCurrentlyActive(void);
			std::string		GetError(ShaderProgramID id);

			std::map<std::string, Graphics::Attribute> AcquireAttributes(ShaderProgramID id);
			std::map<std::string, Graphics::Uniform> AcquireUniforms(ShaderProgramID id);
			std::map<std::string, Graphics::UniformBlock> AcquireUniformBlocks(ShaderProgramID id);
			std::map<ShaderType, Graphics::UniformSubroutine> AcquireUniformSubroutines(ShaderProgramID id, const std::vector<ShaderType>& shaders);
		}

		// ~ Buffers ~ //
		namespace VAO
		{
			VAOID	Create(void);
			void	Delete(VAOID id);
			void	bind(VAOID id);
			void	Unbind();
		}
		namespace VBO
		{
			VBOID	Create(void);
			void	Delete(VBOID id);
			void	bind(VBOID id);
			void	Unbind(void);
			void	BindData(ptrdiff_t length, void* data, BufferUsage usage);
			void	BindSubData(int OffsetBytes, int SizeBytes, void* data);

			void	SetVertexAttribState(uint32_t bufferIndex, bool state);
			void	SetVertexAttrib(uint32_t bufferIndex, int valueCount, DataType datatype, uint32_t strideSize, uint32_t strideOffset, bool normalized);
			void	SetVertexAttribDivisor(uint32_t bufferIndex, uint32_t divisor);
		}
		namespace EBO
		{
			EBOID	Create(void);
			void	Delete(EBOID id);
			void	bind(EBOID id);
			void	Unbind(void);
			void	BindData(ptrdiff_t length, void* data, BufferUsage usage);
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

		// ~ Texture ~ //
		namespace Texture
		{
			TextureID	Create(void);
			void		Delete(TextureID id);
			void		bind(TextureType type, TextureID textureID);
			void		Unbind(TextureType type);
			TextureID	GetCurrentlyBound(void);
			void		SetActiveLevel(TextureLevel level);

			void		CreateStorage(TextureType type, uint32_t levels, TextureFormat format, uint32_t width, uint32_t height);
			void		SetStorage(TextureType type, uint32_t width, uint32_t height, TextureChannelType channelType, TexturePixelType pixeltype, const void* pixels);
			void		SetStorage(CubemapFace type, uint32_t width, uint32_t height, TextureChannelType channelType, TexturePixelType pixeltype, const void* pixels);

			void		GenerateMipmap(TextureType type);
			void		SetWrapping(TextureType type, TextureWrapping wrap);
			void		SetFiltering(TextureType type, TextureFilter filter, bool mipmapping);
			void		SetAnistropic(TextureType type, AnisotropicMode aniso);
			void		SetBorderColor(TextureType type, const Color4F& color);
			void		SetPixelPackAlignment(PixelAlignment align);
			void		SetPixelUnpackAlignment(PixelAlignment align);

			//RawArray<uint8_t> ReadPixels(const Vxl::BaseTexture& texture, int x, int y, int w, int h);
		}
		
		// ~ Renderbuffer ~ //
		namespace RenderBuffer
		{
			RenderBufferID	Create(void);
			void			Delete(RenderBufferID id);
			void			bind(RenderBufferID id);
			void			Unbind(void);
			void			SetStorage(TextureFormat format, int width, int height);
			RenderBufferID	GetCurrentlyBound(void);
		}

		// ~ Framebuffer Object ~ //
		namespace FramebufferObject
		{
			FramebufferObjectID Create(void);
			void				Delete(FramebufferObjectID id);
			void				bind(FramebufferObjectID id);
			void				DrawBuffers(std::vector<uint32_t> attachments);
			//void				DrawBuffers(uint32_t attachmentCount);
			void				DrawBuffer(uint32_t attachmentIndex);
			void				Unbind(void);
			bool				CheckStatus(void);
			FramebufferObjectID	GetCurrentlyBound(void);
			void AttachRenderTexture(const Vxl::RenderTexture& texture, uint32_t attachmentIndex);
			void AttachRenderTextureAsDepth(const Vxl::RenderTexture& texture);
			void DetachRenderTexture(uint32_t attachmentIndex);
			void AttachRenderBuffer(const Vxl::RenderBuffer& texture, uint32_t attachmentIndex);
			void AttachRenderBufferAsDepth(const Vxl::RenderBuffer& texture);
			void DetachRenderBuffer(uint32_t attachmentIndex);

			RawArray<uint8_t>	ReadPixels(const Vxl::RenderTexture& texture, uint32_t attachmentIndex, int x, int y, int w, int h);
			RawArray<uint8_t>	ReadPixels(const Vxl::RenderBuffer& texture, uint32_t attachmentIndex, int x, int y, int w, int h);
			void BlitColor(FramebufferObjectID source, FramebufferObjectID destination, int width, int height, uint32_t srcAttachment, uint32_t destAttachment);
			void BlitDepth(FramebufferObjectID source, FramebufferObjectID destination, int width, int height);
		}

		// ~ UBO ~ //
		namespace UBO
		{
			UBOID	Create(uint32_t slot, uint32_t totalBytes, BufferUsage usage);
			void	Delete(UBOID id);
			void	bind(UBOID id);
			void	Unbind(void);
			void	UpdateBuffer(void* buffer, uint32_t totalBytes, uint32_t offset);
		}

		// ~ Queries ~ //
		namespace Query
		{
			enum class Type
			{
				SAMPLES_PASSED = 0,
				ANY_SAMPLES_PASSED,
				ANY_SAMPLES_PASSED_CONSERVATIVE,
				PRIMITIVES_GENERATED,
				TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
				TIME_ELAPSED
			};

			QueryID		Create(void);
			void		Delete(QueryID id);

			void		Start(QueryID id, Type type);
			void		End(Type type);
			bool		CheckFinished(QueryID id);
			uint64_t	GetResult(QueryID id);
		}
	};

}