// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"

#include "../math/Color.h"
#include "../utilities/Types.h"
#include "../utilities/Util.h"
#include "../utilities/Containers.h"
#include "../utilities/Macros.h"

#include <vector>
#include <Windows.h>

namespace Vxl
{
	class RenderBuffer;
	class RenderBufferDepth;
	class RenderTexture;
	class RenderTextureDepth;

	class RenderTarget
	{
		DISALLOW_COPY_AND_ASSIGN(RenderTarget);
		friend class FramebufferObject;
	private:
		enum class Type
		{
			NONE,
			RENDERTEXTURE,
			RENDERBUFFER
		};

		uint32_t	m_assetIndex = -1;
		Type		m_type = Type::NONE;
	public:
		RenderTarget(bool isDepth = false)
			: m_isDepth(isDepth)
		{}

		const bool			m_isDepth = false;
		
		bool isEmpty(void) const
		{
			return m_assetIndex == -1;
		}
		bool isRenderTexture(void) const
		{
			return m_type == Type::RENDERTEXTURE;
		}
		bool isRenderBuffer(void) const
		{
			return m_type == Type::RENDERBUFFER;
		}

		RenderTexture*		getRenderTexture(void) const;
		RenderTextureDepth*	getRenderTextureDepth(void) const;
		RenderBuffer*		getRenderBuffer(void) const;
		RenderBufferDepth*	getRenderBufferDepth(void) const;
		TextureFormat		getFormatType(void) const;
		inline uint32_t		getAssetIndex(void) const
		{
			return m_assetIndex;
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
		std::string			m_name;
		FramebufferObjectID	m_id = -1;
		Color4F				m_clearColor = Color4F(0,0,0,0);
		ClearMode			m_clearMode = ClearMode::COLOR;
		uint32_t			m_width = 1;
		uint32_t			m_height = 1;
		bool				m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		mutable std::map<uint32_t, RenderTarget>	m_textures;
		RenderTarget			m_depth = RenderTarget(true);
		std::vector<uint32_t>	m_attachmentOrder;

		// Utility
		void updateAttachmentOrder();
		void updateClearMode(TextureDepthFormat format);

		void load();
		void unload();

		// Constructor
		FramebufferObject(const std::string& name);

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
		inline bool isFullscreen(void) const { return m_fullscreen; }
		bool		checkFBOStatus();
		inline std::string getName(void) const
		{
			return m_name;
		}

		// Binding related
		void bind();
		static void unbind();

		void bindTexture(uint32_t index, TextureLevel layer);
		void bindDepth(TextureLevel layer);

		// Clears the color from an entire Texture
		void clearBuffers();
		void clearBuffer(uint32_t attachmentIndex);

		// Attachments
		void setRenderTexture(
			uint32_t _attachmentIndex,
			RenderTextureIndex _index
		);
		void setRenderBuffer(
			uint32_t _attachmentIndex,
			RenderBufferIndex _index
		);
		void removeAttachment(uint32_t _attachmentIndex);
		// Re-attaches all textures/buffers back to current FBO
		void reloadAttachments();
		void reloadDepth();
		// Attachment is still part of FBO class, it just doesn't get drawn to
		void disableAttachment(
			uint32_t _attachmentIndex
		);
		void enableAttachment(
			uint32_t _attachmentIndex
		);

		// Editing Depth Info
		void setRenderTextureDepth(
			RenderTextureDepthIndex _index
		);
		void setRenderBufferDepth(
			RenderBufferDepthIndex _index
		);
		void removeDepth(void);

		// Utility //
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

		bool isAttachmentRenderTexture(uint32_t _attachmentIndex) const;
		bool isAttachmentRenderBuffer(uint32_t _attachmentIndex) const;
		bool isDepthRenderTexture(void) const;
		bool isDepthRenderBuffer(void) const;

		inline const std::vector<uint32_t>&				getAttachmentIndices()
		{
			return m_attachmentOrder;
		}
		inline const std::map<uint32_t, RenderTarget>&	getAttachments()
		{
			return m_textures;
		}
		inline const RenderTarget&						getDepth()
		{
			return m_depth;
		}

		bool hasAttachment(uint32_t _attachmentIndex) const;
		bool hasDepth(void) const;

		RenderTextureIndex getRenderTexture(
			uint32_t _attachmentIndex
		);
		RenderBufferIndex getRenderBuffer(
			uint32_t _attachmentIndex
		);
		RenderTextureDepthIndex getRenderTextureDepth();
		RenderBufferDepthIndex getRenderBufferDepth();
		
	};

}