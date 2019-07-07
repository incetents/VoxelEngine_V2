// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../rendering/Enums.h"

namespace Vxl
{
	class RenderBuffer
	{
	private:
		GLuint	m_id = -1;
		int		m_width;
		int		m_height;
		Format_Type	 m_formatType;
		Channel_Type m_channelType;
		Pixel_Type	 m_pixelType;
		int			 m_channelCount;
	public:
		RenderBuffer(const RenderBuffer&) = delete;

		// Channel/Pixel used only for reading
		RenderBuffer(
			int Width, int Height,
			Format_Type FormatType,
			Channel_Type ChannelType,
			Pixel_Type PixelType
		);
		~RenderBuffer();

		void Bind(void) const;
		static void Unbind(void);

		inline GLuint	GetID(void) const
		{
			return m_id;
		}
		inline int		GetWidth(void) const
		{
			return m_width;
		}
		inline int		GetHeight(void) const
		{
			return m_height;
		}
		inline Format_Type	GetFormatType(void) const
		{
			return m_formatType;
		}
		inline Channel_Type GetChannelType(void) const
		{
			return m_channelType;
		}
		inline Pixel_Type	GetPixelType(void) const
		{
			return m_pixelType;
		}
		inline int			GetChannelCount(void) const
		{
			return m_channelCount;
		}
	};

}