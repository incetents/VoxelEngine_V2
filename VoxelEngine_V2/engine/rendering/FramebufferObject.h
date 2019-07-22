// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"

#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"
#include "../utilities/Containers.h"
#include "../utilities/Macros.h"

#include <vector>
#include <Windows.h>

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

	class FramebufferAttachment
	{
		Attachment_Type	m_type;
		bool			m_empty = true;
	public:
		FramebufferAttachment(
			const std::string& name,
			Attachment_Type type,
			TextureFormat formatType,
			TextureChannelType channelType,
			TexturePixelType pixelType
		)
			: m_name(name), m_type(type), m_formatType(formatType), m_channelType(channelType), m_pixelType(pixelType)
		{}
		~FramebufferAttachment()
		{
			unload();
		}

		const std::string	m_name;
		const TextureFormat	m_formatType;
		const TextureChannelType	m_channelType;
		const TexturePixelType	m_pixelType;

		// Attachment can only be one type
		union
		{
			RenderTexture* m_renderTexture;
			RenderBuffer* m_renderBuffer;
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

		uint32_t			GetID(void) const;
		TextureFormat		GetFormatType(void) const;
		TextureChannelType	GetChannelType(void) const;
		TexturePixelType	GetPixelType(void) const;
		int					GetChannelCount(void) const;

		void Bind();
		void Unbind();
	};

	class FramebufferObject : public Asset<FramebufferObject>
	{
		friend class RenderManager;
	private:
		

		enum class ClearMode
		{
			COLOR,
			COLOR_DEPTH,
			COLOR_STENCIL,
			COLOR_DEPTH_STENCIL
		};

	private:

		// Fbo
		const std::string	m_name;
		FramebufferObjectID	m_id = -1;
		Color4F				m_clearColor = Color4F(0,0,0,0);
		ClearMode			m_clearMode = ClearMode::COLOR;
		uint32_t			m_width = 0;
		uint32_t			m_height = 0;
		bool				m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		std::vector<FramebufferAttachment*>	m_textures;
		uint8_t								m_textureCount = 0;
		FramebufferAttachment*				m_depth = nullptr;

		// Utility
		void load();
		void unload();

		//bool checkFBOStatus();
		void bindFBO();
		
		// Constructor
		FramebufferObject(const std::string& name);
	public:
		// Creator
		static FramebufferObject* Create(const std::string& name);

		~FramebufferObject();

		inline void complete()
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

		inline uint32_t GetID(void) const
		{
			return m_id;
		}
		inline uint32_t GetWidth(void) const
		{
			return m_width;
		}
		inline uint32_t GetHeight(void) const
		{
			return m_height;
		}
		inline uint8_t GetAttachmentCount(void) const
		{
			return m_textureCount;
		}

		// Clears the color from an entire Texture
		void clearBuffers();
		void clearBuffer(uint32_t attachmentIndex);

		void addTexture(
			const std::string& name,
			TextureFormat FormatType		= TextureFormat::RGBA8,
			Attachment_Type fboRenderType = Attachment_Type::TEXTURE
		);
		void addDepth(
			TextureDepthFormat depthFormatType,
			Attachment_Type fboRenderType
		);

		// Enable fbo attachments at runtime
		void enableAttachment(uint32_t attachmentIndex);
		// Disable fbo attachments at runtime
		void disableAttachment(uint32_t attachmentIndex);

		void bind();
		void bind(uint32_t viewportX, uint32_t viewportY, uint32_t viewportW, uint32_t viewportH);
		static void unbind();

		void bindTexture(uint32_t index, TextureLevel layer);
		void bindDepth(TextureLevel layer);

		void blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment);
		void blitDepth(const FramebufferObject& destFBO);

		void generateMipmaps(unsigned int textureIndex);

		RawArray<uint8_t> readPixels(uint32_t textureIndex, int x, int y, int w, int h);
		RawArray<uint8_t> readPixelsFromMouse(uint32_t textureIndex, int w, int h);

		RawArray<uint8_t> readDepthPixels(int x, int y, int w, int h);
		RawArray<uint8_t> readDepthPixelsFromMouse(int w, int h);
	};
}