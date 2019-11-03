// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../rendering/Graphics.h"
#include "../textures/BaseTexture.h"

namespace Vxl
{
	RenderBuffer::RenderBuffer(
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
		: m_width(Width), m_height(Height), m_formatType(FormatType), m_channelType(Graphics::GetChannelType(FormatType)), m_pixelType(PixelType)
	{
		m_channelCount = Graphics::GetChannelCount(FormatType);
		if (m_channelCount == -1)
			m_channelCount = Graphics::GetChannelCount(FormatType);

		m_id = Graphics::RenderBuffer::Create();

		// Storage
		Bind();
		UpdateStorage();
		Unbind();
	}
	void RenderBuffer::UpdateStorage()
	{
		Graphics::RenderBuffer::SetStorage(m_formatType, m_width, m_height);
	}

	RenderBuffer::~RenderBuffer()
	{
		Graphics::RenderBuffer::Delete(m_id);
	}

	// Reload internal flags
	void RenderBuffer::RecreateStorage(uint32_t width, uint32_t height, TextureFormat format)
	{
		// Texture is immutable, destroy it and create a new one
		Unbind();
		Graphics::RenderBuffer::Delete(m_id);
		m_id = Graphics::RenderBuffer::Create();
		Bind();

		// Fix values
		m_width = (int)width;
		m_height = (int)height;
		m_formatType = format;

		// Update gl values
		UpdateStorage();
		Unbind();
	}
	void RenderBuffer::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, name);
	}

	void RenderBuffer::Bind(void) const
	{
		Graphics::RenderBuffer::Bind(m_id);
	}
	void RenderBuffer::Unbind(void)
	{
		Graphics::RenderBuffer::Unbind();
	}

	void RenderBuffer::Copy(const BaseTexture& _texture)
	{
		// Sizes and Formats must be identical
		VXL_ASSERT((int)m_width == (int)_texture.GetWidth(), "Copy Texture Error [Widths do not match]");
		VXL_ASSERT((int)m_height == (int)_texture.GetHeight(), "Copy Texture Error [Heights do not match]");
		VXL_ASSERT(m_formatType == _texture.GetFormatType(), "Copy Texture Error [Formats do not match]");

		Graphics::CopyTexture(
			m_id, TextureType::RENDERBUFFER,
			_texture.GetID(), _texture.GetType(),
			m_width, m_height
		);
	}
	void RenderBuffer::Copy(const RenderBuffer& _texture)
	{
		// Sizes and Formats must be identical
		VXL_ASSERT((int)m_width == (int)_texture.GetWidth(), "Copy Texture Error [Widths do not match]");
		VXL_ASSERT((int)m_height == (int)_texture.GetHeight(), "Copy Texture Error [Heights do not match]");
		VXL_ASSERT(m_formatType == _texture.GetFormatType(), "Copy Texture Error [Formats do not match]");

		Graphics::CopyTexture(
			m_id, TextureType::RENDERBUFFER,
			_texture.GetID(), TextureType::RENDERBUFFER,
			m_width, m_height
		);
	}

	RenderBufferDepth::RenderBufferDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
		: RenderBuffer(Width, Height, Graphics::GetFormat(FormatType), Graphics::GetPixelData(FormatType)), m_depthFormat(FormatType)
	{}
}