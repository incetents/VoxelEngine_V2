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

	class FramebufferObject : public Asset<FramebufferObject>
	{
		DISALLOW_COPY_AND_ASSIGN(FramebufferObject);
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
		mutable std::map<uint32_t, RenderTarget>	m_textures;
		std::vector<uint32_t>				m_attachmentOrder;
		RenderTarget						m_depth;

		void updateAttachmentOrder();
		void updateClearMode(TextureDepthFormat format);

		void load();
		void unload();

		// Constructor
		FramebufferObject(const std::string& name);
	public:
		// Creator
		static FramebufferObject* Create(const std::string& name);

		~FramebufferObject();

		// Sizing
		inline void SetClearColor(Color4F c)
		{
			m_clearColor = c;
		}
		inline void SetSize(unsigned int Width, unsigned int Height)
		{
			m_width = Width;
			m_height = Height;
			m_fullscreen = false;
		}
		void		SetSizeToWindowSize(); // Size will copy Window's resolution

		// Utility
		bool HasAttachment(uint32_t _attachmentIndex) const
		{
			return m_textures.find(_attachmentIndex) != m_textures.end();
		}

		// Binding related
		void Bind();
		void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
		static void Unbind();

		// Clears the color from an entire Texture
		void ClearBuffers();
		void ClearBuffer(uint32_t attachmentIndex);

		// Editing Attachment Info
		void SetAttachment(
			uint32_t _attachmentIndex,
			RenderTexture* _renderTexture
		);
		void SetAttachment(
			uint32_t _attachmentIndex,
			RenderBuffer* _renderbuffer
		);
		void RemoveAttachment(
			uint32_t _attachmentIndex
		);
		// Re-attaches all textures/buffers back to current FBO
		void ReloadAttachments();
		void ReloadDepth();
		// Get Attachment Info
		RenderTexture* GetAttachmentRenderTexture(
			uint32_t _attachmentIndex
		);
		RenderBuffer* GetAttachmentRenderBuffer(
			uint32_t _attachmentIndex
		);
		uint32_t GetAttachmentTextureID(
			uint32_t _attachmentIndex
		);
		inline std::vector<uint32_t> GetAttachmentIndices()
		{
			return m_attachmentOrder;
		}
		// Attachment is still part of FBO class, it just doesn't get drawn to
		void DisableAttachment(
			uint32_t _attachmentIndex
		);
		void EnableAttachment(
			uint32_t _attachmentIndex
		);

		// Editing Depth Info
		void SetDepth(
			RenderTextureDepth* _depth
		);
		void SetDepth(
			RenderBufferDepth* _depth
		);
		void RemoveDepth(void);
		// Get Depth Info
		RenderTexture* GetDepthRenderTexture();
		RenderBuffer* GetDepthRenderBuffer();
		uint32_t GetDepthTextureID();

		// Utility //
		bool checkFBOStatus();

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
	};

}