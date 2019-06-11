// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include "glUtil.h"
#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"
#include "../utilities/Containers.h"

//#include "Texture.h"
#include <vector>

namespace Vxl
{
	class RenderBuffer;
	class RenderTexture;

	enum class FBORenderType
	{
		BUFFER,
		TEXTURE
	};

	class FramebufferObject : public Asset<FramebufferObject>
	{
		friend class RenderManager;
	private:
		class FBOTexture
		{
			FBORenderType	m_type;
			bool			m_empty = true;
		public:
			FBOTexture(
				const std::string& name,
				FBORenderType type,
				Format_Type formatType,
				Channel_Type channelType,
				Pixel_Type pixelType
			)
				: m_name(name), m_type(type), m_formatType(formatType), m_channelType(channelType), m_pixelType(pixelType)
			{}
			~FBOTexture()
			{
				unload();
			}

			const std::string	m_name;
			const Format_Type	m_formatType;
			const Channel_Type	m_channelType;
			const Pixel_Type	m_pixelType;

			union
			{
				RenderTexture* m_texture;
				RenderBuffer* m_buffer;
			};

			void load(int Width, int Height);
			void unload();

			inline bool isRenderTexture(void) const
			{
				return m_type == FBORenderType::TEXTURE;
			}
			inline bool isRenderBuffer(void) const
			{
				return m_type == FBORenderType::BUFFER;
			}

			GLuint			GetID(void) const;
			Format_Type		GetFormatType(void) const;
			Channel_Type	GetChannelType(void) const;
			Pixel_Type		GetPixelType(void) const;
			int				GetChannelCount(void) const;

			void Bind();
			void Unbind();
		};
	private:
		// Fbo
		const std::string m_name;
		GLuint	m_id = -1;
		Color4F m_clearColor = Color4F(0,0,0,0);
		UINT    m_clearBuffers = (GLbitfield)Buffer_Bit_Type::COLOR | (GLbitfield)Buffer_Bit_Type::DEPTH | (GLbitfield)Buffer_Bit_Type::STENCIL;
		UINT	m_width = 0;
		UINT	m_height = 0;
		bool	m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		std::vector<FBOTexture*>	m_textures;
		GLuint						m_textureCount = 0;
		FBOTexture*					m_depth = nullptr;
		// Tracker //
		static GLuint m_boundID;

		// Utility
		void load();
		void unload();

		bool checkFBOStatus();
		void bindFBO();
		
		// Constructor
		FramebufferObject(const std::string& name);
	public:
		// Creator
		static FramebufferObject* Create(const std::string& name);

		~FramebufferObject();

		inline void init()
		{
			assert(m_width > 0 && m_width > 0);
			load();
		}
		inline void setClearColor(Color4F c)
		{
			m_clearColor = c;
		}
		inline void setClearBuffer(Buffer_Bit_Type b1, Buffer_Bit_Type b2 = Buffer_Bit_Type::NONE, Buffer_Bit_Type b3 = Buffer_Bit_Type::NONE, Buffer_Bit_Type b4 = Buffer_Bit_Type::NONE)
		{
			m_clearBuffers = (GLbitfield)b1 | (GLbitfield)b2 | (GLbitfield)b3 | (GLbitfield)b4;
		}
		inline void setSize(unsigned int Width, unsigned int Height)
		{
			m_width = Width;
			m_height = Height;
			m_fullscreen = false;
		}
		// Size will copy Window's resolution
		void setSizeToWindowSize();

		inline GLuint GetID(void) const
		{
			return m_id;
		}
		inline GLuint GetWidth(void) const
		{
			return m_width;
		}
		inline GLuint GetHeight(void) const
		{
			return m_height;
		}

		// Clears the color from an entire Texture
		void clearBuffers();
		void clearBuffer(unsigned int index);

		void addTexture(
			const std::string& name,
			Format_Type FormatType		= Format_Type::RGBA8,
			FBORenderType fboRenderType = FBORenderType::TEXTURE
		);
		void addDepth(
			DepthFormat_Type depthFormatType,
			FBORenderType fboRenderType
		);

		void bind();
		void bind(UINT viewportX, UINT viewportY, UINT viewportW, UINT viewportH);
		static void unbind();

		void bindTexture(int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);

		void blitDepth(const FramebufferObject& fbo);

		void generateMipmaps(unsigned int textureIndex);

		RawArray<GLubyte> readPixels(u_int textureIndex, int x, int y, int w, int h);
		RawArray<GLubyte> readDepthPixel(int x, int y, int w, int h);
	};
}