// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderBuffer.h"

#include "../opengl/glUtil.h"

namespace Vxl
{
	RenderBuffer::RenderBuffer(
		int Width, int Height,
		Format_Type FormatType, Channel_Type ChannelType, Pixel_Type PixelType
	)
		: m_width(Width), m_height(Height), m_formatType(FormatType), m_channelType(ChannelType), m_pixelType(PixelType)
	{
		m_channelCount = (glUtil::getChannelCount(FormatType, ChannelType));

		glGenRenderbuffers(1, &m_id);

		glBindRenderbuffer(GL_RENDERBUFFER, m_id);
		glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)FormatType, Width, Height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

	}
	RenderBuffer::~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &m_id);
	}

	void RenderBuffer::Bind(void) const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_id);
	}
	void RenderBuffer::Unbind(void)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}