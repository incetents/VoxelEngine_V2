// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../rendering/Graphics.h"
#include "../textures/BaseTexture.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	RenderBuffer::RenderBuffer(
		const std::string& name,
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
		: m_name(name), m_width(Width), m_height(Height), m_formatType(FormatType), m_channelType(Graphics::GetChannelType(FormatType)), m_pixelType(PixelType)
	{
		m_channelCount = Graphics::GetChannelCount(FormatType);
		if (m_channelCount == -1)
			m_channelCount = Graphics::GetChannelCount(FormatType);

		m_id = Graphics::RenderBuffer::Create();

		// Storage
		bind();
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
		{
			auto newID = Graphics::RenderBuffer::Create();
			Graphics::RenderBuffer::Delete(m_id);
			m_id = newID;
		}
		bind();

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
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "Renderbuffer_" + name);
	}

	void RenderBuffer::bind(void) const
	{
		Graphics::RenderBuffer::bind(m_id);
	}
	void RenderBuffer::Unbind(void)
	{
		Graphics::RenderBuffer::Unbind();
	}

	//	void RenderBuffer::Copy(const BaseTexture& _texture)
	//	{
	//		// Sizes and Formats must be identical
	//		VXL_ASSERT((int)m_width == (int)_texture.getWidth(), "Copy Texture Error [Widths do not match]");
	//		VXL_ASSERT((int)m_height == (int)_texture.getHeight(), "Copy Texture Error [Heights do not match]");
	//		VXL_ASSERT(m_formatType == _texture.getFormatType(), "Copy Texture Error [Formats do not match]");
	//	
	//		Graphics::CopyTexture(
	//			m_id, TextureType::RENDERBUFFER,
	//			_texture.getID(), _texture.GetType(),
	//			m_width, m_height
	//		);
	//	}
	//	void RenderBuffer::Copy(const RenderBuffer& _texture)
	//	{
	//		// Sizes and Formats must be identical
	//		VXL_ASSERT((int)m_width == (int)_texture.getWidth(), "Copy Texture Error [Widths do not match]");
	//		VXL_ASSERT((int)m_height == (int)_texture.getHeight(), "Copy Texture Error [Heights do not match]");
	//		VXL_ASSERT(m_formatType == _texture.getFormatType(), "Copy Texture Error [Formats do not match]");
	//	
	//		Graphics::CopyTexture(
	//			m_id, TextureType::RENDERBUFFER,
	//			_texture.getID(), TextureType::RENDERBUFFER,
	//			m_width, m_height
	//		);
	//	}

	RenderBufferDepth::RenderBufferDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
		: RenderBuffer("Depth", Width, Height, Graphics::GetFormat(FormatType), Graphics::GetPixelData(FormatType)), m_depthFormat(FormatType)
	{}
}