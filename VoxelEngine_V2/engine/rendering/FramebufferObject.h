// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>

#include "glUtil.h"
#include "Graphics.h"

#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"
#include "../utilities/Containers.h"
#include "../utilities/Macros.h"

//#include "Texture.h"
#include <vector>

namespace Vxl
{
	class RenderBuffer;
	class RenderTexture;

	enum class Attachment_Type
	{
		// Render Buffer [Cannot bind like a texture]
		BUFFER,
		// Render Texture
		TEXTURE
	};

	class FramebufferObject : public Asset<FramebufferObject>
	{
		friend class RenderManager;
	private:
		class Attachment
		{
			Attachment_Type	m_type;
			bool			m_empty = true;
		public:
			Attachment(
				const std::string& name,
				Attachment_Type type,
				Format_Type formatType,
				Channel_Type channelType,
				Pixel_Type pixelType
			)
				: m_name(name), m_type(type), m_formatType(formatType), m_channelType(channelType), m_pixelType(pixelType)
			{}
			~Attachment()
			{
				unload();
			}

			const std::string	m_name;
			const Format_Type	m_formatType;
			const Channel_Type	m_channelType;
			const Pixel_Type	m_pixelType;

			// Attachment can only be one type
			union
			{
				RenderTexture* m_texture;
				RenderBuffer* m_buffer;
			};

			void load(int Width, int Height);
			void unload();

			inline bool isRenderTexture(void) const
			{
				return m_type == Attachment_Type::TEXTURE;
			}
			inline bool isRenderBuffer(void) const
			{
				return m_type == Attachment_Type::BUFFER;
			}

			GLuint			GetID(void) const;
			Format_Type		GetFormatType(void) const;
			Channel_Type	GetChannelType(void) const;
			Pixel_Type		GetPixelType(void) const;
			int				GetChannelCount(void) const;

			void Bind();
			void Unbind();
		};

		enum class ClearMode
		{
			COLOR,
			COLOR_DEPTH,
			COLOR_STENCIL,
			COLOR_DEPTH_STENCIL
		};

	private:

		// Fbo
		const std::string m_name;
		GLuint	m_id = -1;
		Color4F m_clearColor = Color4F(0,0,0,0);
		ClearMode m_clearMode = ClearMode::COLOR;
		UINT	m_width = 0;
		UINT	m_height = 0;
		bool	m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		std::vector<Attachment*>	m_textures;
		GLuint						m_textureCount = 0;
		Attachment*					m_depth = nullptr;
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
			VXL_ASSERT(m_width > 0 && m_width > 0, "Invalid FBO Init Size");
			load();
		}
		inline void setClearColor(Color4F c)
		{
			m_clearColor = c;
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
			Attachment_Type fboRenderType = Attachment_Type::TEXTURE
		);
		void addDepth(
			DepthFormat_Type depthFormatType,
			Attachment_Type fboRenderType
		);

		void bind();
		void bind(UINT viewportX, UINT viewportY, UINT viewportW, UINT viewportH);
		static void unbind();

		void bindTexture(unsigned int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);

		void blitDepth(const FramebufferObject& fbo);

		void generateMipmaps(unsigned int textureIndex);

		RawArray<GLubyte> readPixels(u_int textureIndex, int x, int y, int w, int h);
		RawArray<GLubyte> readPixelsFromMouse(u_int textureIndex, int w, int h);

		RawArray<GLubyte> readDepthPixels(int x, int y, int w, int h);
		RawArray<GLubyte> readDepthPixelsFromMouse(int w, int h);
	};
}