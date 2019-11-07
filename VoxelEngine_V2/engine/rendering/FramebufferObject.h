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

	class RenderTarget
	{
	public:
		// Type
		enum class Type
		{
			// ERROR
			NONE = 0,
			// Render Buffer [Cannot bind like a texture]
			BUFFER,
			// Render Texture
			TEXTURE
		};

	private:
		// Data
		Type m_type = Type::NONE;

		union // [ Can only be one object ]
		{
			RenderTexture* m_renderTexture;
			RenderBuffer*  m_renderBuffer;
		};

	public:
		void Set(RenderTexture* _renderTexture)
		{
			m_type = Type::TEXTURE;
			m_renderTexture = _renderTexture;
		}
		void Set(RenderBuffer* _renderBuffer)
		{
			m_type = Type::BUFFER;
			m_renderBuffer = _renderBuffer;
		}
		void Remove()
		{
			m_type = Type::NONE;
			m_renderTexture = nullptr;
		}

		RenderTexture* GetRenderTexture(void) const
		{
			VXL_ASSERT(m_type == Type::TEXTURE, "Incorrect FBO Attachment Type for Getter");
			return m_renderTexture;
		}
		RenderBuffer* GetRenderBuffer(void) const
		{
			VXL_ASSERT(m_type == Type::BUFFER, "Incorrect FBO Attachment Type for Getter");
			return m_renderBuffer;
		}
		uint32_t GetID(void) const;

		TextureFormat GetFormatType(void) const;

		Type GetType(void) const
		{
			return m_type;
		}
		bool IsRenderTexture(void) const
		{
			return m_type == Type::TEXTURE;
		}
		bool IsRenderBuffer(void) const
		{
			return m_type == Type::BUFFER;
		}
		bool IsUnused(void) const
		{
			return m_type == Type::NONE;
		}
	};

	class FramebufferObject
	{
		DISALLOW_COPY_AND_ASSIGN(FramebufferObject);
		friend class Assets;
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
		FramebufferObjectID	m_id = -1;
		Color4F				m_clearColor = Color4F(0,0,0,0);
		ClearMode			m_clearMode = ClearMode::COLOR;
		uint32_t			m_width = 1;
		uint32_t			m_height = 1;
		bool				m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		mutable std::map<uint32_t, RenderTarget>	m_textures;
		std::vector<uint32_t>				m_attachmentOrder;
		RenderTarget						m_depth;

		// Utility
		void updateAttachmentOrder();
		void updateClearMode(TextureDepthFormat format);

		void load();
		void unload();

		// Constructor
		FramebufferObject();

	public:
		// ~
		~FramebufferObject();

		// Utility
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
		void		setSizeToViewportSize(); // Size will copy Window's resolution
		void		setGLName(const std::string& name);
		bool		hasAttachment(uint32_t _attachmentIndex) const
		{
			return m_textures.find(_attachmentIndex) != m_textures.end();
		}

		// Binding related
		void bind();
		//void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
		static void unbind();

		// Clears the color from an entire Texture
		void clearBuffers();
		void clearBuffer(uint32_t attachmentIndex);

		// Editing Attachment Info
		void setAttachment(
			uint32_t _attachmentIndex,
			RenderTexture* _renderTexture
		);
		void setAttachment(
			uint32_t _attachmentIndex,
			RenderBuffer* _renderbuffer
		);
		void removeAttachment(
			uint32_t _attachmentIndex
		);
		// Re-attaches all textures/buffers back to current FBO
		void reloadAttachments();
		void reloadDepth();
		// Get Attachment Info
		RenderTexture* getAttachmentRenderTexture(
			uint32_t _attachmentIndex
		);
		RenderBuffer* getAttachmentRenderBuffer(
			uint32_t _attachmentIndex
		);
		uint32_t getAttachmentTextureID(
			uint32_t _attachmentIndex
		);
		inline const std::vector<uint32_t>& getAttachmentIndices()
		{
			return m_attachmentOrder;
		}
		// Attachment is still part of FBO class, it just doesn't get drawn to
		void disableAttachment(
			uint32_t _attachmentIndex
		);
		void enableAttachment(
			uint32_t _attachmentIndex
		);

		// Editing Depth Info
		void setDepth(
			RenderTextureDepth* _depth
		);
		void setDepth(
			RenderBufferDepth* _depth
		);
		void removeDepth(void);
		// Get Depth Info
		RenderTexture* getDepthRenderTexture();
		RenderBuffer* getDepthRenderBuffer();
		uint32_t getDepthTextureID();

		// Utility //
		bool checkFBOStatus();
		inline bool isFullscreen(void) const { return m_fullscreen; }

		void bindTexture(uint32_t index, TextureLevel layer);
		void bindDepth(TextureLevel layer);

		void blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment);
		void blitDepth(const FramebufferObject& destFBO);

		void updateMipmaps(uint32_t _attachmentIndex);

		RawArray<uint8_t> readPixels(uint32_t _attachmentIndex, int x, int y, int w, int h);
		RawArray<uint8_t> readPixelsFromMouse(uint32_t _attachmentIndex, int w, int h);

		RawArray<uint8_t> readDepthPixels(int x, int y, int w, int h);
		RawArray<uint8_t> readDepthPixelsFromMouse(int w, int h);

		// Getters
		inline uint32_t getID(void) const
		{
			return m_id;
		}
		inline uint32_t getWidth(void) const
		{
			return m_width;
		}
		inline uint32_t getHeight(void) const
		{
			return m_height;
		}
	};

}