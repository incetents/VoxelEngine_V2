// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../rendering/Graphics.h"

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
		Graphics::RenderBuffer::Bind(m_id);
		Graphics::RenderBuffer::SetStorage(FormatType, m_width, m_height);
		Graphics::RenderBuffer::Unbind();


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