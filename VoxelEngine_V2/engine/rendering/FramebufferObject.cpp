// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"
#include "RenderBuffer.h"
#include "Graphics.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../rendering/UBO.h"
#include "../utilities/Logger.h"
#include "../utilities/Asset.h"
#include "../window/window.h"
#include "../input/Input.h"

namespace Vxl
{
	RenderTexture* RenderTarget::getRenderTexture(void) const
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_type == RenderTarget::Type::RENDERTEXTURE, "RenderTarget not a renderTexture");
		VXL_RETURN_NULLPTR_ON_FAIL(m_isDepth == false, "cannot get RenderTexture for RenderTextureDepth");
		return Assets::getRenderTexture(m_assetIndex);
	}
	RenderTextureDepth*	RenderTarget::getRenderTextureDepth(void) const
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_type == RenderTarget::Type::RENDERTEXTURE, "RenderTarget not a renderTexture");
		VXL_RETURN_NULLPTR_ON_FAIL(m_isDepth == true, "cannot get RenderTextureDepth for RenderTexture");
		return Assets::getRenderTextureDepth(m_assetIndex);
	}
	RenderBuffer* RenderTarget::getRenderBuffer(void) const
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_type == RenderTarget::Type::RENDERBUFFER, "RenderTarget not a renderBuffer");
		VXL_RETURN_NULLPTR_ON_FAIL(m_isDepth == false, "cannot get RenderBuffer for RenderBufferDepth");
		return Assets::getRenderBuffer(m_assetIndex);
	}
	RenderBufferDepth* RenderTarget::getRenderBufferDepth(void) const
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_type == RenderTarget::Type::RENDERBUFFER, "RenderTarget not a renderBuffer");
		VXL_RETURN_NULLPTR_ON_FAIL(m_isDepth == true, "cannot get RenderBufferDepth for RenderBuffer");
		return Assets::getRenderBufferDepth(m_assetIndex);
	}
	TextureFormat RenderTarget::getFormatType(void) const
	{
		if (m_type == Type::RENDERTEXTURE)
		{
			RenderTexture* rt = Assets::getRenderTexture(m_assetIndex);
			VXL_ASSERT(rt, "Missing RenderTexture");
			if (rt)
				return rt->getFormatType();
		}
		else if (m_type == Type::RENDERBUFFER)
		{
			RenderBuffer* rb = Assets::getRenderBuffer(m_assetIndex);
			VXL_ASSERT(rb, "Missing RenderTexture");
			if (rb)
				return rb->getFormatType();
		}
	
		return TextureFormat::RGBA8;
	}

	FramebufferObject::FramebufferObject(const std::string& name)
		: m_depth(true), m_name(name)
	{
		load();
	}

	FramebufferObject::~FramebufferObject()
	{
		unload();
	}

	void FramebufferObject::updateAttachmentOrder()
	{
		m_attachmentOrder.clear();
		m_attachmentOrder.resize(m_textures.size());
		int i = 0;
		for (const auto& tex : m_textures)
			m_attachmentOrder[i++] = tex.first;
	}
	void FramebufferObject::updateClearMode(TextureDepthFormat format)
	{
		// Update clear mode
		switch (format)
		{
		case TextureDepthFormat::STENCIL8:
			m_clearMode = ClearMode::COLOR_STENCIL;
			break;
		case TextureDepthFormat::DEPTH24_STENCIL8:
		case TextureDepthFormat::DEPTH32F_STENCIL8:
			m_clearMode = ClearMode::COLOR_DEPTH_STENCIL;
			break;
		default:
			m_clearMode = ClearMode::COLOR_DEPTH;
			break;
		}
	}

	void FramebufferObject::load()
	{
		// Create FBO
		m_id = Graphics::FramebufferObject::Create();
		
		Graphics::FramebufferObject::bind(m_id);
		Graphics::FramebufferObject::Unbind();
	}
	void FramebufferObject::unload()
	{
		Graphics::FramebufferObject::Delete(m_id);
		m_id = -1;
	}

	void FramebufferObject::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::FRAMEBUFFER, m_id, "FBO_" + name);
	}

	void FramebufferObject::setSizeToViewportSize()
	{
		m_width = Window.GetViewportWidth();
		m_height = Window.GetViewportHeight();
		m_fullscreen = true;
	}
	bool FramebufferObject::checkFBOStatus()
	{
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to check its status");

		if (!Graphics::FramebufferObject::CheckStatus())
		{
			Logger.error("Failed to create FBO");
			return false;
		}
		return true;
	}

	void FramebufferObject::bind()
	{
		Graphics::FramebufferObject::bind(m_id);

		Graphics::SetViewport(0, 0, m_width, m_height);

		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);

		UBOManager.BindFBOSize(*this);
	}
	void FramebufferObject::unbind()
	{
		Graphics::FramebufferObject::bind(0);
	}

	void FramebufferObject::bindTexture(uint32_t index, TextureLevel layer)
	{
		VXL_ASSERT(hasAttachment(index), "FBO, bindTexture index out of bounds");
		VXL_ASSERT(m_textures[index].isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		RenderTexture* rt = Assets::getRenderTexture(m_textures[index].m_assetIndex);
		rt->bind();
	}
	void FramebufferObject::bindDepth(TextureLevel layer)
	{
		VXL_ASSERT(m_depth.isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		RenderTextureDepth* rtd = Assets::getRenderTextureDepth(m_depth.m_assetIndex);
		rtd->bind();
	}

	void FramebufferObject::clearBuffers()
	{
		Graphics::SetClearColor(m_clearColor);

		switch (m_clearMode)
		{
		case ClearMode::COLOR:
			Graphics::ClearBuffer(BufferBit::COLOR);
			break;
		case ClearMode::COLOR_DEPTH:
			Graphics::ClearBuffer(BufferBit::COLOR, BufferBit::DEPTH);
			break;
		case ClearMode::COLOR_STENCIL:
			Graphics::ClearBuffer(BufferBit::COLOR, BufferBit::STENCIL);
			break;
		case ClearMode::COLOR_DEPTH_STENCIL:
			Graphics::ClearAllBuffers();
			break;
		default:
			VXL_ASSERT(false, "Unknown ClearMode flag for FBO");
		}

	}
	void FramebufferObject::clearBuffer(uint32_t _attachmentIndex)
	{
		VXL_ASSERT(hasAttachment(_attachmentIndex), "FBO, Index out of bounds");
		Graphics::ClearBufferFBOAttachment(_attachmentIndex, m_clearColor);
	}

	void FramebufferObject::setRenderTexture(
		uint32_t _attachmentIndex,
		RenderTextureIndex _index
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		RenderTexture* renderData = Assets::getRenderTexture(_index);
		VXL_RETURN_ON_FAIL(renderData, "RenderTexture missing");

		m_textures[_attachmentIndex].m_assetIndex = _index;
		m_textures[_attachmentIndex].m_type = RenderTarget::Type::RENDERTEXTURE;

		updateAttachmentOrder();

		Graphics::FramebufferObject::AttachRenderTexture(*renderData, _attachmentIndex);
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::setRenderBuffer(
		uint32_t _attachmentIndex,
		RenderBufferIndex _index
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		RenderBuffer* renderData = Assets::getRenderBuffer(_index);
		VXL_RETURN_ON_FAIL(renderData, "RenderBuffer missing");

		m_textures[_attachmentIndex].m_assetIndex = _index;
		m_textures[_attachmentIndex].m_type = RenderTarget::Type::RENDERBUFFER;

		updateAttachmentOrder();

		Graphics::FramebufferObject::AttachRenderBuffer(*renderData, _attachmentIndex);
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::removeAttachment(uint32_t _attachmentIndex)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		// Disconnect attachment from FBO
		disableAttachment(_attachmentIndex);
		// Remove reference to attachment
		m_textures.erase(_attachmentIndex);

		// Update attachment order
		updateAttachmentOrder();
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::reloadAttachments()
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to reload attachments");

		for (const auto& texture : m_textures)
		{
			if (texture.second.m_type == RenderTarget::Type::RENDERTEXTURE)
			{
				RenderTexture* render = Assets::getRenderTexture(texture.second.m_assetIndex);
				render->RecreateStorage(getWidth(), getHeight(), render->getFormatType(), render->getPixelType());

				Graphics::FramebufferObject::AttachRenderTexture(*render, texture.first);
			}
			else if (texture.second.m_type == RenderTarget::Type::RENDERBUFFER)
			{
				RenderBuffer* render = Assets::getRenderBuffer(texture.second.m_assetIndex);
				render->RecreateStorage(getWidth(), getHeight(), render->getFormatType());

				Graphics::FramebufferObject::AttachRenderBuffer(*render, texture.first);
			}
		}
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::reloadDepth()
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to reload attachments");

		if (m_depth.m_type == RenderTarget::Type::RENDERTEXTURE)
		{
			RenderTextureDepth* render = Assets::getRenderTextureDepth(m_depth.m_assetIndex);
			render->RecreateStorage(getWidth(), getHeight(), render->getFormatType(), render->getPixelType());

			Graphics::FramebufferObject::AttachRenderTextureAsDepth(*render);
		}
		else if (m_depth.m_type == RenderTarget::Type::RENDERBUFFER)
		{
			RenderBufferDepth* render = Assets::getRenderBufferDepth(m_depth.m_assetIndex);
			render->RecreateStorage(getWidth(), getHeight(), render->getFormatType());

			Graphics::FramebufferObject::AttachRenderBufferAsDepth(*render);
		}
	}
	void FramebufferObject::disableAttachment(
		uint32_t _attachmentIndex
	)
	{
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		
		switch (m_textures[_attachmentIndex].m_type)
		{
		case RenderTarget::Type::RENDERTEXTURE:
		{
			RenderTexture* render = Assets::getRenderTexture(m_textures[_attachmentIndex].m_assetIndex);
			Graphics::FramebufferObject::DetachRenderTexture(render->getID());
			break;
		}
		case RenderTarget::Type::RENDERBUFFER:
		{
			RenderBuffer* render = Assets::getRenderBuffer(m_textures[_attachmentIndex].m_assetIndex);
			Graphics::FramebufferObject::DetachRenderBuffer(render->getID());
			break;
		}
		default:
			return;
		}
	}
	void FramebufferObject::enableAttachment(
		uint32_t _attachmentIndex
	)
	{
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		
		switch (m_textures[_attachmentIndex].m_type)
		{
		case RenderTarget::Type::RENDERTEXTURE:
		{
			RenderTexture* renderData = Assets::getRenderTexture(m_textures[_attachmentIndex].m_assetIndex);
			VXL_RETURN_ON_FAIL(renderData, "Missing RenderTexture");

			Graphics::FramebufferObject::AttachRenderTexture(*renderData, _attachmentIndex);
			break;
		}
		case RenderTarget::Type::RENDERBUFFER:
		{
			RenderBuffer* renderData = Assets::getRenderBuffer(m_textures[_attachmentIndex].m_assetIndex);
			VXL_RETURN_ON_FAIL(renderData, "Missing RenderBuffer");

			Graphics::FramebufferObject::AttachRenderBuffer(*renderData, _attachmentIndex);
			break;
		}
		default:
			return;
		}
	}

	void FramebufferObject::setRenderTextureDepth(
		RenderTextureDepthIndex _index
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");

		RenderTextureDepth* rt = Assets::getRenderTextureDepth(_index);
		VXL_RETURN_ON_FAIL(rt, "Missing RenderTextureDepth");

		m_depth.m_assetIndex = _index;
		m_depth.m_type = RenderTarget::Type::RENDERTEXTURE;

		updateClearMode(rt->m_depthFormat);
		Graphics::FramebufferObject::AttachRenderTextureAsDepth(*rt);
	}
	void FramebufferObject::setRenderBufferDepth(
		RenderBufferDepthIndex _index
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");

		RenderBufferDepth* rt = Assets::getRenderBufferDepth(_index);
		VXL_RETURN_ON_FAIL(rt, "Missing RenderBufferDepth");

		m_depth.m_assetIndex = _index;
		m_depth.m_type = RenderTarget::Type::RENDERBUFFER;

		updateClearMode(rt->m_depthFormat);
		Graphics::FramebufferObject::AttachRenderBufferAsDepth(*rt);
	}

	void FramebufferObject::removeDepth(void)
	{
		m_depth.m_assetIndex = -1;
		m_depth.m_type = RenderTarget::Type::NONE;
		m_clearMode = ClearMode::COLOR;
	}
	
	void FramebufferObject::blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment)
	{
		// Both fbos must have attachments
		VXL_ASSERT(hasAttachment(srcAttachment), "SrcAttachment Not Found for blitColor()");
		VXL_ASSERT(destFBO.hasAttachment(destAttachment), "SrcAttachment Not Found for blitColor()");
		// Must have matching formats
		VXL_ASSERT(m_textures[srcAttachment].getFormatType() == destFBO.m_textures[destAttachment].getFormatType(), "FBO, blitColor doesn't have matching formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitColor doesn't have matching sizes");
		
		Graphics::FramebufferObject::BlitColor(m_id, destFBO.m_id, m_width, m_height, srcAttachment, destAttachment);
	}

	void FramebufferObject::blitDepth(const FramebufferObject& destFBO)
	{
		// Both fbos must have depth
		VXL_ASSERT(m_depth.m_isDepth && destFBO.m_depth.m_isDepth, "FBO, blitDepth doesn't have depth on both FBO's");
		// Must have matching depth formats
		VXL_ASSERT(m_depth.getFormatType() == destFBO.m_depth.getFormatType(), "FBO, blitDepth doesn't have matching depth formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitDepth doesn't have matching sizes");

		Graphics::FramebufferObject::BlitDepth(m_id, destFBO.m_id, m_width, m_height);
	}

	// Generates mipmap of current texture
	void FramebufferObject::updateMipmaps(uint32_t _attachmentIndex)
	{
		VXL_ASSERT(hasAttachment(_attachmentIndex), "FBO, index out of bounds");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be found before generating mipmaps");
		VXL_ASSERT(m_textures[_attachmentIndex].isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		// Create mipmapping for Fbo Texture
		RenderTexture* rt = Assets::getRenderTexture(m_textures[_attachmentIndex].m_assetIndex);
		rt->UpdateMipmapping();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<uint8_t> FramebufferObject::readPixels(uint32_t _attachmentIndex, int x, int y, int w, int h)
	{
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0 || x >= (int)m_width || y >= (int)m_height)
			return RawArray<uint8_t>();

		//std::cout << x << ' ' << y << std::endl;

		VXL_ASSERT(hasAttachment(_attachmentIndex), "FBO readpixels, missing attachment to read from");

		if (m_textures[_attachmentIndex].isRenderTexture())
		{
			RenderTexture* render = Assets::getRenderTexture(m_textures[_attachmentIndex].m_assetIndex);
			return Graphics::FramebufferObject::ReadPixels(*render, _attachmentIndex, x, y, w, h);
		}
		else
		{
			RenderBuffer* render = Assets::getRenderBuffer(m_textures[_attachmentIndex].m_assetIndex);
			return Graphics::FramebufferObject::ReadPixels(*render, _attachmentIndex, x, y, w, h);
		}
	}
	RawArray<uint8_t> FramebufferObject::readPixelsFromMouse(uint32_t _attachmentIndex, int w, int h)
	{
		return readPixels(
			_attachmentIndex, (int)(Input.getMousePosViewportX() * m_width), (int)(Input.getMousePosViewportY() * m_height), w, h);
	}

	RawArray<uint8_t> FramebufferObject::readDepthPixels(int x, int y, int w, int h)
	{
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0 || x >= (int)m_width || y >= (int)m_height)
			return RawArray<uint8_t>();

		VXL_ASSERT(m_depth.m_isDepth, "FBO readpixels: no depth buffer found");

		// Ignore if x,y coordinates are outside FBO range
		if (!m_depth.m_isDepth)
			return RawArray<uint8_t>();

		if (m_depth.isRenderTexture())
		{
			RenderTexture* render = Assets::getRenderTexture(m_depth.m_assetIndex);
			return Graphics::FramebufferObject::ReadPixels(*render, -1, x, y, w, h);
		}
		else
		{
			RenderBuffer* render = Assets::getRenderBuffer(m_depth.m_assetIndex);
			return Graphics::FramebufferObject::ReadPixels(*render, -1, x, y, w, h);
		}
	}
	RawArray<uint8_t> FramebufferObject::readDepthPixelsFromMouse(int w, int h)
	{
		return readDepthPixels(
			(int)(Input.getMousePosViewportX() * m_width), (int)(Input.getMousePosViewportY() * m_height), w, h);
	}

	bool FramebufferObject::isAttachmentRenderTexture(uint32_t _attachmentIndex) const
	{
		auto it = m_textures.find(_attachmentIndex);
		if (it == m_textures.end())
			return false;

		return m_textures[_attachmentIndex].isRenderTexture();
	}
	bool FramebufferObject::isAttachmentRenderBuffer(uint32_t _attachmentIndex) const
	{
		auto it = m_textures.find(_attachmentIndex);
		if (it == m_textures.end())
			return false;

		return m_textures[_attachmentIndex].isRenderBuffer();
	}
	bool FramebufferObject::isDepthRenderTexture(void) const
	{
		return m_depth.isRenderTexture();
	}
	bool FramebufferObject::isDepthRenderBuffer(void) const
	{
		return m_depth.isRenderBuffer();
	}
	bool FramebufferObject::hasAttachment(uint32_t _attachmentIndex) const
	{
		auto it = m_textures.find(_attachmentIndex);
		if (it == m_textures.end())
			return false;

		return !m_textures[_attachmentIndex].isEmpty();
	}
	bool FramebufferObject::hasDepth(void) const
	{
		return !m_depth.isEmpty();
	}

	RenderTextureIndex FramebufferObject::getRenderTexture(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_ZERO_ON_FAIL(hasAttachment(_attachmentIndex), "Missing Attachment");
		VXL_RETURN_ZERO_ON_FAIL(m_textures[_attachmentIndex].isRenderTexture(), "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].m_assetIndex;
	}
	RenderBufferIndex FramebufferObject::getRenderBuffer(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_ZERO_ON_FAIL(hasAttachment(_attachmentIndex), "Missing Attachment");
		VXL_RETURN_ZERO_ON_FAIL(m_textures[_attachmentIndex].isRenderBuffer(), "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].m_assetIndex;
	}
	RenderTextureDepthIndex FramebufferObject::getRenderTextureDepth()
	{
		VXL_RETURN_ZERO_ON_FAIL(hasDepth(), "Missing Depth");
		VXL_RETURN_ZERO_ON_FAIL(m_depth.isRenderTexture(), "Incorrect Depth Type");
		return m_depth.m_assetIndex;
	}
	RenderBufferDepthIndex FramebufferObject::getRenderBufferDepth()
	{
		VXL_RETURN_ZERO_ON_FAIL(hasDepth(), "Missing Depth");
		VXL_RETURN_ZERO_ON_FAIL(m_depth.isRenderBuffer(), "Incorrect Depth Type");
		return m_depth.m_assetIndex;
	}

	//	RenderTexture* FramebufferObject::getAttachmentRenderTexture(
	//		uint32_t _attachmentIndex
	//	)
	//	{
	//		VXL_RETURN_NULLPTR_ON_FAIL(hasAttachment(_attachmentIndex), "Missing Attachment");
	//		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].m_type == RenderTargetType::RENDERTEXTURE, "Incorrect Attachment Info");
	//		return m_textures[_attachmentIndex].getRenderTexture();
	//	}
	//	RenderBuffer* FramebufferObject::getAttachmentRenderBuffer(
	//		uint32_t _attachmentIndex
	//	)
	//	{
	//		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].m_type == RenderTargetType::RENDERBUFFER, "Incorrect Attachment Info");
	//		return m_textures[_attachmentIndex].getRenderBuffer();
	//	}

}
