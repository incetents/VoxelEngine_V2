// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"
#include "RenderBuffer.h"
#include "Graphics.h"

#include "../textures/Texture.h"
#include "../textures/RenderTexture.h"
#include "../utilities/logger.h"
#include "../window/window.h"
#include "../input/Input.h"

namespace Vxl
{
	//GLuint FramebufferObject::m_boundID = 0;

	void FramebufferAttachment::load(int Width, int Height)
	{
		unload();

		if (m_type == Attachment_Type::TEXTURE)
		{
			m_renderTexture = new RenderTexture(Width, Height, m_formatType, m_channelType, m_pixelType);
			Graphics::SetGLName(ObjectType::TEXTURE, m_renderTexture->GetID(), "FBO_" + m_name + "_Tex_" + m_name);
		}
		else
		{
			m_renderBuffer = new RenderBuffer(Width, Height, m_formatType, m_channelType, m_pixelType);
			Graphics::SetGLName(ObjectType::TEXTURE, m_renderBuffer->GetID(), "FBO_" + m_name + "_RenderBuf_" + m_name);
		}


		m_empty = false;
	}

	void FramebufferAttachment::unload()
	{
		if (!m_empty)
		{
			if (m_type == Attachment_Type::BUFFER)
				delete m_renderBuffer;
			else if (m_type == Attachment_Type::TEXTURE)
				delete m_renderTexture;

			m_empty = true;
		}
	}

	uint32_t FramebufferAttachment::GetID(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_renderBuffer->GetID();

		return m_renderTexture->GetID();
	}
	TextureFormat	FramebufferAttachment::GetFormatType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_renderBuffer->GetFormatType();

		return m_renderTexture->GetFormatType();
	}
	TextureChannelType FramebufferAttachment::GetChannelType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_renderBuffer->GetChannelType();

		return m_renderTexture->GetChannelType();
	}
	TexturePixelType FramebufferAttachment::GetPixelType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_renderBuffer->GetPixelType();

		return m_renderTexture->GetPixelType();
	}
	int FramebufferAttachment::GetChannelCount(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_renderBuffer->GetChannelCount();

		return m_renderTexture->GetChannelCount();
	}

	void FramebufferAttachment::Bind()
	{
		if (m_type == Attachment_Type::BUFFER)
			m_renderBuffer->Bind();
		else
			m_renderTexture->Bind();
	}
	void FramebufferAttachment::Unbind()
	{
		if (m_type == Attachment_Type::BUFFER)
			m_renderBuffer->Unbind();
		else
			m_renderTexture->Unbind();
	}

	FramebufferObject::FramebufferObject(const std::string& name)
		: m_name(name)
	{
		
	}

	FramebufferObject* FramebufferObject::Create(
		const std::string& name
	){
		FramebufferObject* _FBO = new FramebufferObject(name);

		AddToDatabase(name, _FBO);
		Message_Created(name, _FBO);

		return _FBO;
	}

	FramebufferObject::~FramebufferObject()
	{
		unload();
	}

	void FramebufferObject::load()
	{
		// Already init
		if (m_id != -1)
		{
			VXL_ASSERT(false, "FBO already init");
			return;
		}

		// Create FBO
		m_id = Graphics::FramebufferObject::Create();
		Graphics::FramebufferObject::Bind(m_id);

		Graphics::SetGLName(ObjectType::FRAMEBUFFER, m_id, "FBO_" + m_name);

		// Color attachments
		for (unsigned int i = 0; i < m_textureCount; i++)
		{
			m_textures[i]->load(m_width, m_height);

			if (m_textures[i]->isRenderTexture())
				Graphics::FramebufferObject::AttachRenderTexture(*m_textures[i]->m_renderTexture, i);
			else
				Graphics::FramebufferObject::AttachRenderBuffer(*m_textures[i]->m_renderBuffer, i);
		}

		// Depth attachment
		if (m_depth != nullptr)
		{
			m_depth->load(m_width, m_height);
			if (m_depth->isRenderTexture())
				Graphics::FramebufferObject::AttachRenderTextureAsDepth(*m_depth->m_renderTexture);
			else
				Graphics::FramebufferObject::AttachRenderBufferAsDepth(*m_depth->m_renderBuffer);
		}

		// Final Check
		if (!Graphics::FramebufferObject::CheckStatus())
			Logger.error("Failed to create FBO: " + m_name);
		//checkFBOStatus();
		Graphics::FramebufferObject::Unbind();
	}
	void FramebufferObject::unload()
	{
		if (m_id != -1)
		{
			Graphics::FramebufferObject::Delete(m_id);
			m_id = -1;

			// Destroy attached textures and depth/stencil
			for (auto& tex : m_textures)
				tex->unload();
			m_depth->unload();
		}
	}

	void FramebufferObject::bindFBO()
	{
		// Don't bind FBO if it hasn't changed
		//if (m_boundID != m_id)
		{
			Graphics::FramebufferObject::Bind(m_id);
			Graphics::FramebufferObject::DrawBuffers(m_textureCount);
		}
		// update bound FBO
		//m_boundID = m_id;
	}
	void FramebufferObject::setSizeToWindowSize()
	{
		m_width = Window.GetResolutionWidth();
		m_height = Window.GetResolutionHeight();
		m_fullscreen = true;
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
	void FramebufferObject::clearBuffer(uint32_t attachmentIndex)
	{
		VXL_ASSERT(attachmentIndex < m_textureCount, "FBO, Index out of bounds");
		Graphics::ClearBufferFBOAttachment(attachmentIndex, m_clearColor);
	}

	void FramebufferObject::addTexture(
		const std::string& name,
		TextureFormat FormatType,
		Attachment_Type fboRenderType
	)
	{
		VXL_ASSERT(m_textureCount < (uint32_t)Graphics::GLMaxFBOColorAttachments, "FBO, too many color attachments");
		
		// Create new render texture
		m_textures.push_back(new FramebufferAttachment(name, fboRenderType, FormatType, TextureChannelType::RGBA, TexturePixelType::UNSIGNED_BYTE));
		m_textureCount++;
	}
	void FramebufferObject::addDepth(TextureDepthFormat depthFormatType, Attachment_Type fboRenderType)
	{
		VXL_ASSERT(m_depth == nullptr, "FBO, cannot add multiple depth/stencil attachments");

		if (depthFormatType == TextureDepthFormat::STENCIL8)
			m_clearMode = ClearMode::COLOR_STENCIL;
		else if (depthFormatType == TextureDepthFormat::DEPTH24_STENCIL8 || depthFormatType == TextureDepthFormat::DEPTH32F_STENCIL8)
			m_clearMode = ClearMode::COLOR_DEPTH_STENCIL;
		else
			m_clearMode = ClearMode::COLOR_DEPTH;

		// Select Pixel/Channel Type [for glReadPixels]
		TexturePixelType PixelType;
		TextureChannelType ChannelType;
		Graphics::GetPixelChannelData(depthFormatType, ChannelType, PixelType);

		m_depth = new FramebufferAttachment("depth", fboRenderType, Graphics::GetFormat(depthFormatType), ChannelType, PixelType);
	}

	void FramebufferObject::bind()
	{
		bindFBO();

		Graphics::SetViewport(0, 0, m_width, m_height);
	}
	void FramebufferObject::bind(uint32_t viewportX, uint32_t viewportY, uint32_t viewportW, uint32_t viewportH)
	{
		bindFBO();

		Graphics::SetViewport(viewportX, viewportY, viewportW, viewportH);
	}
	void FramebufferObject::unbind()
	{
		Graphics::FramebufferObject::Unbind();
	}

	void FramebufferObject::bindTexture(uint32_t index, TextureLevel layer)
	{
		VXL_ASSERT(index < m_textureCount, "FBO, bindTexture index out of bounds");
		VXL_ASSERT(m_textures[index]->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(TextureLevel layer)
	{
		VXL_ASSERT(m_depth->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		Graphics::Texture::SetActiveLevel(layer);
		m_depth->Bind();
	}
	
	void FramebufferObject::blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment)
	{
		// Both fbos must have attachments
		VXL_ASSERT(srcAttachment < m_textureCount, "SrcAttachment Not Found for blitColor()");
		VXL_ASSERT(destAttachment < destFBO.m_textureCount, "SrcAttachment Not Found for blitColor()");
		// Must have matching formats
		//VXL_ASSERT(m_textures[srcAttachment]->GetFormatType() == destFBO.m_textures[srcAttachment]->GetFormatType(), "FBO, blitColor doesn't have matching formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitColor doesn't have matching sizes");
		
		Graphics::FramebufferObject::BlitColor(m_id, destFBO.m_id, m_width, m_height, srcAttachment, destAttachment);
	}

	void FramebufferObject::blitDepth(const FramebufferObject& destFBO)
	{
		// Both fbos must have depth
		VXL_ASSERT(m_depth != nullptr && destFBO.m_depth != nullptr, "FBO, blitDepth doesn't have depth on both FBO's");
		// Must have matching formats
		VXL_ASSERT(m_depth->GetFormatType() == destFBO.m_depth->GetFormatType(), "FBO, blitDepth doesn't have matching depth formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == destFBO.m_width && m_height == destFBO.m_height, "FBO, blitDepth doesn't have matching sizes");

		Graphics::FramebufferObject::BlitDepth(m_id, destFBO.m_id, m_width, m_height);
	}

	// Generates mipmap of current texture
	void FramebufferObject::generateMipmaps(unsigned int textureIndex)
	{
		VXL_ASSERT(textureIndex < m_textureCount, "FBO, index out of bounds");
		//VXL_ASSERT(m_id == m_boundID, "FBO" + m_name + "must be bound before generating mipmaps for its color attachments");
		VXL_ASSERT(m_textures[textureIndex]->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		// Create mipmapping for Fbo Texture
		m_textures[textureIndex]->m_renderTexture->updateMipmapping();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<uint8_t> FramebufferObject::readPixels(uint32_t attachmentIndex, int x, int y, int w, int h)
	{
		VXL_ASSERT(attachmentIndex < m_textureCount, "FBO readpixels: index out of bounds");
		
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0)
			return RawArray<uint8_t>();

		return Graphics::FramebufferObject::ReadPixels(*m_textures[attachmentIndex], attachmentIndex, x, y, w, h);
	}
	RawArray<uint8_t> FramebufferObject::readPixelsFromMouse(uint32_t attachmentIndex, int w, int h)
	{
		float px, py;
		px = Input.getMousePosViewportX();  // [0 -> 1] horizontally across viewport
		py = Input.getMousePosViewportY(true);  // [0 -> 1] vertically across viewport

		// Move position from [0->1] into [0->width] and [0->height]
		px *= m_width;
		py *= m_height;

		return readPixels(attachmentIndex, (int)px, (int)py, w, h);
	}

	RawArray<uint8_t> FramebufferObject::readDepthPixels(int x, int y, int w, int h)
	{
		return Graphics::FramebufferObject::ReadPixels(*m_depth, -1, x, y, w, h);
	}
	RawArray<uint8_t> FramebufferObject::readDepthPixelsFromMouse(int w, int h)
	{
		float px, py;
		px = Input.getMousePosViewportX();  // [0 -> 1] horizontally across viewport
		py = Input.getMousePosViewportY(true);  // [0 -> 1] vertically across viewport

		// Move position from [0->1] into [0->width] and [0->height]
		px *= m_width;
		py *= m_height;

		return readDepthPixels((int)px, (int)py, w, h);
	}
}
