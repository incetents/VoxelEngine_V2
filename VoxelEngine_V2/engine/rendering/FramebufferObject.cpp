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
	uint32_t RenderTarget::GetID(void) const
	{
		switch (m_type)
		{
		case Type::BUFFER:
			return m_renderBuffer->GetID();
		case Type::TEXTURE:
			return m_renderTexture->GetID();
		default:
			VXL_ASSERT(false, "Cannot get ID of empty FBO attachment");
			return 0;
		}
	}

	TextureFormat RenderTarget::GetFormatType(void) const
	{
		if (m_type == Type::TEXTURE)
			return m_renderTexture->GetFormatType();
		else if (m_type == Type::BUFFER)
			return m_renderBuffer->GetFormatType();

		return TextureFormat::NONE;
	}

	FramebufferObject::FramebufferObject()
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
		
		Graphics::FramebufferObject::Bind(m_id);
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

	void FramebufferObject::bind()
	{
		Graphics::FramebufferObject::Bind(m_id);

		Graphics::SetViewport(0, 0, m_width, m_height);

		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);

		UBOManager.BindFBOSize(*this);
	}
	//void FramebufferObject::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	//{
	//	Graphics::SetViewport(x, y, w, h);
	//}
	void FramebufferObject::unbind()
	{
		Graphics::FramebufferObject::Bind(0);
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

	void FramebufferObject::setAttachment(
		uint32_t _attachmentIndex,
		RenderTexture* _renderTexture
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		m_textures[_attachmentIndex].Set(_renderTexture);
		updateAttachmentOrder();

		Graphics::FramebufferObject::AttachRenderTexture(*_renderTexture, _attachmentIndex);
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::setAttachment(
		uint32_t _attachmentIndex,
		RenderBuffer* _renderbuffer
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		m_textures[_attachmentIndex].Set(_renderbuffer);
		updateAttachmentOrder();

		Graphics::FramebufferObject::AttachRenderBuffer(*_renderbuffer, _attachmentIndex);
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::removeAttachment(
		uint32_t _attachmentIndex
	)
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
			if (texture.second.IsRenderTexture())
			{
				RenderTexture* render = texture.second.GetRenderTexture();
				render->RecreateStorage(getWidth(), getHeight(), render->GetFormatType(), render->GetPixelType());

				Graphics::FramebufferObject::AttachRenderTexture(*texture.second.GetRenderTexture(), texture.first);
			}
			else if (texture.second.IsRenderBuffer())
			{
				auto render = texture.second.GetRenderBuffer();
				render->RecreateStorage(getWidth(), getHeight(), render->GetFormatType());

				Graphics::FramebufferObject::AttachRenderBuffer(*texture.second.GetRenderBuffer(), texture.first);
			}
		}
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::reloadDepth()
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to reload attachments");

		if (m_depth.IsRenderTexture())
		{
			auto render = m_depth.GetRenderTexture();
			render->RecreateStorage(getWidth(), getHeight(), render->GetFormatType(), render->GetPixelType());

			Graphics::FramebufferObject::AttachRenderTextureAsDepth(*m_depth.GetRenderTexture());
		}
		else if (m_depth.IsRenderBuffer())
		{
			auto render = m_depth.GetRenderBuffer();
			render->RecreateStorage(getWidth(), getHeight(), render->GetFormatType());

			Graphics::FramebufferObject::AttachRenderBufferAsDepth(*m_depth.GetRenderBuffer());
		}
	}
	RenderTexture* FramebufferObject::getAttachmentRenderTexture(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_NULLPTR_ON_FAIL(hasAttachment(_attachmentIndex), "Missing Attachment");
		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].GetType() == RenderTarget::Type::TEXTURE, "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].GetRenderTexture();
	}
	RenderBuffer* FramebufferObject::getAttachmentRenderBuffer(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].GetType() == RenderTarget::Type::BUFFER, "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].GetRenderBuffer();
	}
	uint32_t FramebufferObject::getAttachmentTextureID(
		uint32_t _attachmentIndex
	)
	{
		return m_textures[_attachmentIndex].GetID();
	}
	void FramebufferObject::disableAttachment(
		uint32_t _attachmentIndex
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		
		switch (m_textures[_attachmentIndex].GetType())
		{
		case RenderTarget::Type::TEXTURE:
			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
			Graphics::FramebufferObject::DetachRenderTexture(_attachmentIndex);
			break;
		case RenderTarget::Type::BUFFER:
			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
			Graphics::FramebufferObject::DetachRenderBuffer(_attachmentIndex);
			break;
		default:
			return;
		}
	}
	void FramebufferObject::enableAttachment(
		uint32_t _attachmentIndex
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		
		switch (m_textures[_attachmentIndex].GetType())
		{
		case RenderTarget::Type::TEXTURE:
			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
			Graphics::FramebufferObject::AttachRenderTexture(*m_textures[_attachmentIndex].GetRenderTexture(), _attachmentIndex);
			break;
		case RenderTarget::Type::BUFFER:
			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to remove attachments");
			Graphics::FramebufferObject::AttachRenderBuffer(*m_textures[_attachmentIndex].GetRenderBuffer(), _attachmentIndex);
			break;
		default:
			return;
		}
	}

	void FramebufferObject::setDepth(RenderTextureDepth* _depth)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(_depth, "RenderTextureDepth is nullptr");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");

		m_depth.Set(_depth);
		updateClearMode(_depth->m_depthFormat);

		Graphics::FramebufferObject::AttachRenderTextureAsDepth(*_depth);
	}
	void FramebufferObject::setDepth(RenderBufferDepth* _depth)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(_depth, "RenderTextureDepth is nullptr");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");

		m_depth.Set(_depth);
		updateClearMode(_depth->m_depthFormat);

		Graphics::FramebufferObject::AttachRenderBufferAsDepth(*_depth);
	}
	void FramebufferObject::removeDepth(void)
	{
		m_depth.Remove();
		m_clearMode = ClearMode::COLOR;
	}
	RenderTexture* FramebufferObject::getDepthRenderTexture()
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_depth.GetType() == RenderTarget::Type::TEXTURE, "Incorrect Depth Info");
		return m_depth.GetRenderTexture();
	}
	RenderBuffer* FramebufferObject::getDepthRenderBuffer()
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_depth.GetType() == RenderTarget::Type::BUFFER, "Incorrect Depth Info");
		return m_depth.GetRenderBuffer();
	}
	uint32_t FramebufferObject::getDepthTextureID()
	{
		return m_depth.GetID();
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

	void FramebufferObject::bindTexture(uint32_t index, TextureLevel layer)
	{
		VXL_ASSERT(hasAttachment(index), "FBO, bindTexture index out of bounds");
		VXL_ASSERT(m_textures[index].IsRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		m_textures[index].GetRenderTexture()->Bind();
	}
	void FramebufferObject::bindDepth(TextureLevel layer)
	{
		VXL_ASSERT(m_depth.IsRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		m_depth.GetRenderTexture()->Bind();
	}
	
	void FramebufferObject::blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment)
	{
		// Both fbos must have attachments
		VXL_ASSERT(hasAttachment(srcAttachment), "SrcAttachment Not Found for blitColor()");
		VXL_ASSERT(destFBO.hasAttachment(destAttachment), "SrcAttachment Not Found for blitColor()");
		// Must have matching formats
		VXL_ASSERT(m_textures[srcAttachment].GetFormatType() == destFBO.m_textures[destAttachment].GetFormatType(), "FBO, blitColor doesn't have matching formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitColor doesn't have matching sizes");
		
		Graphics::FramebufferObject::BlitColor(m_id, destFBO.m_id, m_width, m_height, srcAttachment, destAttachment);
	}

	void FramebufferObject::blitDepth(const FramebufferObject& destFBO)
	{
		// Both fbos must have depth
		VXL_ASSERT(!m_depth.IsUnused() && !destFBO.m_depth.IsUnused(), "FBO, blitDepth doesn't have depth on both FBO's");
		// Must have matching depth formats
		VXL_ASSERT(m_depth.GetFormatType() == destFBO.m_depth.GetFormatType(), "FBO, blitDepth doesn't have matching depth formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitDepth doesn't have matching sizes");

		Graphics::FramebufferObject::BlitDepth(m_id, destFBO.m_id, m_width, m_height);
	}

	// Generates mipmap of current texture
	void FramebufferObject::updateMipmaps(uint32_t _attachmentIndex)
	{
		VXL_ASSERT(hasAttachment(_attachmentIndex), "FBO, index out of bounds");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be found before generating mipmaps");
		VXL_ASSERT(m_textures[_attachmentIndex].IsRenderTexture(), "FBO, bindTexture is not RenderTexture");

		// Create mipmapping for Fbo Texture
		m_textures[_attachmentIndex].GetRenderTexture()->UpdateMipmapping();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<uint8_t> FramebufferObject::readPixels(uint32_t _attachmentIndex, int x, int y, int w, int h)
	{
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0 || x >= (int)m_width || y >= (int)m_height)
			return RawArray<uint8_t>();

		//std::cout << x << ' ' << y << std::endl;

		VXL_ASSERT(hasAttachment(_attachmentIndex), "FBO readpixels, missing attachment to read from");

		if(m_textures[_attachmentIndex].IsRenderTexture())
			return Graphics::FramebufferObject::ReadPixels(*m_textures[_attachmentIndex].GetRenderTexture(), _attachmentIndex, x, y, w, h);
		
		return Graphics::FramebufferObject::ReadPixels(*m_textures[_attachmentIndex].GetRenderBuffer(), _attachmentIndex, x, y, w, h);
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

		VXL_ASSERT(!m_depth.IsUnused(), "FBO readpixels: no depth buffer found");

		// Ignore if x,y coordinates are outside FBO range
		if (m_depth.IsUnused())
			return RawArray<uint8_t>();

		if(m_depth.IsRenderTexture())
			return Graphics::FramebufferObject::ReadPixels(*m_depth.GetRenderTexture(), -1, x, y, w, h);

		return Graphics::FramebufferObject::ReadPixels(*m_depth.GetRenderBuffer(), -1, x, y, w, h);
	}
	RawArray<uint8_t> FramebufferObject::readDepthPixelsFromMouse(int w, int h)
	{
		return readDepthPixels(
			(int)(Input.getMousePosViewportX() * m_width), (int)(Input.getMousePosViewportY() * m_height), w, h);
	}
}
