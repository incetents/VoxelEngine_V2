// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../rendering/Graphics.h"

namespace Vxl
{
	RenderBuffer::RenderBuffer(
		std::string name,
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
		Graphics::RenderBuffer::Bind(m_id);
		Graphics::RenderBuffer::SetStorage(FormatType, m_width, m_height);
		Graphics::RenderBuffer::Unbind();

	}

	RenderBuffer* RenderBuffer::Create(
		std::string name,
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
	{
		RenderBuffer* _buffer = new RenderBuffer(name, Width, Height, FormatType, PixelType);

		if(name.empty())
			AddUnnamedAsset(_buffer, AssetMessage::CREATED);
		else
			AddNamedAsset(name, _buffer, AssetMessage::CREATED);

		if (_buffer->GetID() == -1)
		{
			Logger.error("RenderBuffer failed to create");
			return nullptr;
		}

		return _buffer;
	}

	RenderBuffer::~RenderBuffer()
	{
		Graphics::RenderBuffer::Delete(m_id);
	}

	void RenderBuffer::Bind(void) const
	{
		Graphics::RenderBuffer::Bind(m_id);
	}
	void RenderBuffer::Unbind(void)
	{
		Graphics::RenderBuffer::Unbind();
	}
}