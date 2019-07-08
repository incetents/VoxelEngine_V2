// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../rendering/glUtil.h"
#include "../rendering/Graphics.h"

namespace Vxl
{
	RenderBuffer::RenderBuffer(
		int Width, int Height,
		TextureFormat FormatType, TextureChannelType ChannelType, TexturePixelType PixelType
	)
		: m_width(Width), m_height(Height), m_formatType(FormatType), m_channelType(ChannelType), m_pixelType(PixelType)
	{
		//m_channelCount = (glUtil::getChannelCount(FormatType, ChannelType));
		m_channelCount = Graphics::GetChannelCount(ChannelType);
		if (m_channelCount == -1)
			m_channelCount = Graphics::GetChannelCount(FormatType);

		m_id = Graphics::RenderBuffer::Create();
		Graphics::RenderBuffer::Bind(m_id);
		Graphics::RenderBuffer::SetStorage(FormatType, m_width, m_height);
		Graphics::RenderBuffer::Unbind();
		//glGenRenderbuffers(1, &m_id);
		//
		//glBindRenderbuffer(GL_RENDERBUFFER, m_id);
		//glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)FormatType, Width, Height);
		//
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	}
	RenderBuffer::~RenderBuffer()
	{
		//glDeleteRenderbuffers(1, &m_id);
		Graphics::RenderBuffer::Delete(m_id);
	}

	void RenderBuffer::Bind(void) const
	{
		//glBindRenderbuffer(GL_RENDERBUFFER, m_id);
		Graphics::RenderBuffer::Bind(m_id);
	}
	void RenderBuffer::Unbind(void)
	{
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
		Graphics::RenderBuffer::Unbind();
	}
}