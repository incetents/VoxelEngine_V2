// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"
#include "RenderBuffer.h"
#include "Graphics.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../rendering/UBO.h"
#include "../utilities/Logger.h"
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
	std::string RenderTarget::GetName(void) const
	{
		switch (m_type)
		{
		case Type::BUFFER:
			return m_renderBuffer->GetName();
		case Type::TEXTURE:
			return m_renderTexture->GetName();
		default:
			VXL_ASSERT(false, "Cannot get name of empty FBO attachment");
			return "";
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

	FramebufferObject::FramebufferObject(const std::string& name)
		: m_name(name)
	{
		load();
	}

	FramebufferObject* FramebufferObject::Create(
		const std::string& name
	){
		FramebufferObject* _FBO = new FramebufferObject(name);

		AddNamedAsset(name, _FBO, AssetMessage::CREATED);

		return _FBO;
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

	void FramebufferObject::load()
	{
		// Create FBO
		m_id = Graphics::FramebufferObject::Create();
		
		Graphics::FramebufferObject::Bind(m_id);
		Graphics::SetGLName(ObjectType::FRAMEBUFFER, m_id, "FBO_" + m_name);
		Graphics::FramebufferObject::Unbind();
	}
	void FramebufferObject::unload()
	{
		Graphics::FramebufferObject::Delete(m_id);
		m_id = -1;
	}

	void FramebufferObject::SetSizeToWindowSize()
	{
		m_width = Window.GetViewportWidth();
		m_height = Window.GetViewportHeight();
		m_fullscreen = true;
	}

	void FramebufferObject::Bind()
	{
		Graphics::FramebufferObject::Bind(m_id);

		Graphics::SetViewport(0, 0, m_width, m_height);

		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);

		UBOManager.BindFBOSize(*this);
	}
	void FramebufferObject::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		Graphics::SetViewport(x, y, w, h);
	}
	void FramebufferObject::Unbind()
	{
		Graphics::FramebufferObject::Bind(0);
	}


	void FramebufferObject::ClearBuffers()
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
	void FramebufferObject::ClearBuffer(uint32_t _attachmentIndex)
	{
		VXL_ASSERT(HasAttachment(_attachmentIndex), "FBO, Index out of bounds");
		Graphics::ClearBufferFBOAttachment(_attachmentIndex, m_clearColor);
	}

	void FramebufferObject::SetAttachment(
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
	void FramebufferObject::SetAttachment(
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
	void FramebufferObject::NewAttachment(
		uint32_t _attachmentIndex,
		RenderTarget::Type _type,
		const std::string& _name,
		TextureFormat _format,
		bool _mipmapping
	)
	{
		if (m_textures.find(_attachmentIndex) != m_textures.end())
		{
			Logger.error("err");
		}

		VXL_ASSERT(m_textures.find(_attachmentIndex) == m_textures.end(), "FBO: cannot create new attachment when one already exists");
		VXL_ASSERT(_type != RenderTarget::Type::NONE, "FBO NewAttachment: Type is not specified");

		if (_type == RenderTarget::Type::TEXTURE)
		{
			SetAttachment(
				_attachmentIndex,
				RenderTexture::Create(
					"FBO_" + m_name + "_Tex_" + _name, m_width, m_height,
					_format, TexturePixelType::UNSIGNED_BYTE, _mipmapping
				)
			);
		}
		else if (_type == RenderTarget::Type::BUFFER)
		{
			VXL_ASSERT(!_mipmapping, "FBO New Attachment: Renderbuffer cannot take mipmap parameter");

			SetAttachment(
				_attachmentIndex,
				RenderBuffer::Create(
					"FBO_" + m_name + "_Tex_" + _name, m_width, m_height,
					_format, TexturePixelType::UNSIGNED_BYTE
				)
			);
		}
	}
	void FramebufferObject::RemoveAttachment(
		uint32_t _attachmentIndex
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit attachments");

		// Disconnect attachment from FBO
		DisableAttachment(_attachmentIndex);
		// Remove reference to attachment
		m_textures.erase(_attachmentIndex);

		// Update attachment order
		updateAttachmentOrder();
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::ReloadAttachments()
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to reload attachments");

		for (auto& texture : m_textures)
		{
			if (texture.second.IsRenderTexture())
			{
				auto render = texture.second.GetRenderTexture();
				render->RecreateStorage(GetWidth(), GetHeight(), render->GetFormatType(), render->GetPixelType());

				Graphics::FramebufferObject::AttachRenderTexture(*texture.second.GetRenderTexture(), texture.first);
			}
			else if (texture.second.IsRenderBuffer())
			{
				auto render = texture.second.GetRenderBuffer();
				render->RecreateStorage(GetWidth(), GetHeight(), render->GetFormatType());

				Graphics::FramebufferObject::AttachRenderBuffer(*texture.second.GetRenderBuffer(), texture.first);
			}
		}
		Graphics::FramebufferObject::DrawBuffers(m_attachmentOrder);
	}
	void FramebufferObject::ReloadDepth()
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to reload attachments");

		if (m_depth.IsRenderTexture())
		{
			auto render = m_depth.GetRenderTexture();
			render->RecreateStorage(GetWidth(), GetHeight(), render->GetFormatType(), render->GetPixelType());

			Graphics::FramebufferObject::AttachRenderTextureAsDepth(*m_depth.GetRenderTexture());
		}
		else if (m_depth.IsRenderBuffer())
		{
			auto render = m_depth.GetRenderBuffer();
			render->RecreateStorage(GetWidth(), GetHeight(), render->GetFormatType());

			Graphics::FramebufferObject::AttachRenderBufferAsDepth(*m_depth.GetRenderBuffer());
		}
	}
	RenderTexture* FramebufferObject::GetAttachmentRenderTexture(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_NULLPTR_ON_FAIL(HasAttachment(_attachmentIndex), "Missing Attachment");
		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].GetType() == RenderTarget::Type::TEXTURE, "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].GetRenderTexture();
	}
	RenderBuffer* FramebufferObject::GetAttachmentRenderBuffer(
		uint32_t _attachmentIndex
	)
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_textures[_attachmentIndex].GetType() == RenderTarget::Type::BUFFER, "Incorrect Attachment Info");
		return m_textures[_attachmentIndex].GetRenderBuffer();
	}
	uint32_t FramebufferObject::GetAttachmentTextureID(
		uint32_t _attachmentIndex
	)
	{
		return m_textures[_attachmentIndex].GetID();
	}
	std::string FramebufferObject::GetAttachmentName(
		uint32_t _attachmentIndex
	)
	{
		return m_textures[_attachmentIndex].GetName();
	}
	void FramebufferObject::DisableAttachment(
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
	void FramebufferObject::EnableAttachment(
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

	void FramebufferObject::SetDepth(
		TextureDepthFormat _depthFormat,
		RenderTarget::Type _type
	)
	{
		VXL_ASSERT(m_id != -1, "FBO not initialized");
		VXL_ASSERT(_type != RenderTarget::Type::NONE, "Incorrect Enum value");

		// If depth already exists, delete it first
		if (!m_depth.IsUnused())
			RemoveDepth();

		if (_type == RenderTarget::Type::TEXTURE)
		{
			RenderTexture* _renderTexture = RenderTexture::Create(
				"FBO_" + m_name + "_Depth",
				m_width, m_height,
				Graphics::GetFormat(_depthFormat),
				Graphics::GetPixelData(_depthFormat)
			);

			m_depth.Set(_renderTexture);

			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");
			Graphics::FramebufferObject::AttachRenderTextureAsDepth(*_renderTexture);

			Graphics::SetGLName(ObjectType::TEXTURE, _renderTexture->GetID(), "FBO_" + m_name + "_Depth");
		}
		else
		{
			RenderBuffer* _renderBuffer = RenderBuffer::Create(
				"FBO_" + m_name + "_Depth",
				m_width, m_height,
				Graphics::GetFormat(_depthFormat),
				Graphics::GetPixelData(_depthFormat)
			);

			m_depth.Set(_renderBuffer);

			VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to edit depth");
			Graphics::FramebufferObject::AttachRenderBufferAsDepth(*_renderBuffer);

			Graphics::SetGLName(ObjectType::TEXTURE, _renderBuffer->GetID(), "FBO_" + m_name + "_Depth");
		}

		// Update clear mode
		if (_depthFormat == TextureDepthFormat::STENCIL8)
			m_clearMode = ClearMode::COLOR_STENCIL;
		else if (_depthFormat == TextureDepthFormat::DEPTH24_STENCIL8 || _depthFormat == TextureDepthFormat::DEPTH32F_STENCIL8)
			m_clearMode = ClearMode::COLOR_DEPTH_STENCIL;
		else
			m_clearMode = ClearMode::COLOR_DEPTH;

	}
	void FramebufferObject::RemoveDepth(void)
	{
		if (m_depth.IsRenderTexture())
			RenderTexture::DeleteNamedAsset(m_depth.GetName());
		else if (m_depth.IsRenderBuffer())
			RenderBuffer::DeleteNamedAsset(m_depth.GetName());

		m_depth.Remove();
		m_clearMode = ClearMode::COLOR;
	}
	RenderTexture* FramebufferObject::GetDepthRenderTexture()
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_depth.GetType() == RenderTarget::Type::TEXTURE, "Incorrect Depth Info");
		return m_depth.GetRenderTexture();
	}
	RenderBuffer* FramebufferObject::GetDepthRenderBuffer()
	{
		VXL_RETURN_NULLPTR_ON_FAIL(m_depth.GetType() == RenderTarget::Type::BUFFER, "Incorrect Depth Info");
		return m_depth.GetRenderBuffer();
	}
	uint32_t FramebufferObject::GetDepthTextureID()
	{
		return m_depth.GetID();
	}
	std::string FramebufferObject::GetDepthtName()
	{
		return m_depth.GetName();
	}
	bool FramebufferObject::checkFBOStatus()
	{
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be bound to check its status");

		if (!Graphics::FramebufferObject::CheckStatus())
		{
			Logger.error("Failed to create FBO: " + m_name);
			return false;
		}
		return true;
	}

	void FramebufferObject::bindTexture(uint32_t index, TextureLevel layer)
	{
		VXL_ASSERT(HasAttachment(index), "FBO, bindTexture index out of bounds");
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
		VXL_ASSERT(HasAttachment(srcAttachment), "SrcAttachment Not Found for blitColor()");
		VXL_ASSERT(destFBO.HasAttachment(destAttachment), "SrcAttachment Not Found for blitColor()");
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
		VXL_ASSERT(HasAttachment(_attachmentIndex), "FBO, index out of bounds");
		VXL_ASSERT(Graphics::FramebufferObject::GetCurrentlyBound() == m_id, "FBO must be found before generating mipmaps");
		VXL_ASSERT(m_textures[_attachmentIndex].IsRenderTexture(), "FBO, bindTexture is not RenderTexture");

		// Create mipmapping for Fbo Texture
		m_textures[_attachmentIndex].GetRenderTexture()->UpdateMipmapping();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<uint8_t> FramebufferObject::readPixels(uint32_t _attachmentIndex, int x, int y, int w, int h)
	{
		bool _HasAttachment = HasAttachment(_attachmentIndex);
		VXL_ASSERT(_HasAttachment, "FBO readpixels: index out of bounds");
		
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0 || !_HasAttachment)
			return RawArray<uint8_t>();

		if(m_textures[_attachmentIndex].IsRenderTexture())
			return Graphics::FramebufferObject::ReadPixels(*m_textures[_attachmentIndex].GetRenderTexture(), _attachmentIndex, x, y, w, h);
		
		return Graphics::FramebufferObject::ReadPixels(*m_textures[_attachmentIndex].GetRenderBuffer(), _attachmentIndex, x, y, w, h);
	}
	RawArray<uint8_t> FramebufferObject::readPixelsFromMouse(uint32_t _attachmentIndex, int w, int h)
	{
		float px, py;
		px = Input.getMousePosViewportX();  // [0 -> 1] horizontally across viewport
		py = Input.getMousePosViewportY(true);  // [0 -> 1] vertically across viewport

		// Move position from [0->1] into [0->width] and [0->height]
		px *= m_width;
		py *= m_height;

		return readPixels(_attachmentIndex, (int)px, (int)py, w, h);
	}

	RawArray<uint8_t> FramebufferObject::readDepthPixels(int x, int y, int w, int h)
	{
		bool _HasDepth = !m_depth.IsUnused();
		VXL_ASSERT(_HasDepth, "FBO readpixels: index out of bounds");
		
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0 || !_HasDepth)
			return RawArray<uint8_t>();

		if(m_depth.IsRenderTexture())
			return Graphics::FramebufferObject::ReadPixels(*m_depth.GetRenderTexture(), -1, x, y, w, h);

		return Graphics::FramebufferObject::ReadPixels(*m_depth.GetRenderBuffer(), -1, x, y, w, h);
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
