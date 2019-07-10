// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"

namespace Vxl
{
	class RenderBuffer
	{
	private:
		uint32_t	m_id = -1;
		int			m_width;
		int			m_height;
		TextureFormat	 m_formatType;
		TextureChannelType m_channelType;
		TexturePixelType	 m_pixelType;
		int			 m_channelCount;
	public:
		RenderBuffer(const RenderBuffer&) = delete;

		// Channel/Pixel used only for reading
		RenderBuffer(
			int Width, int Height,
			TextureFormat FormatType,
			TextureChannelType ChannelType,
			TexturePixelType PixelType
		);
		~RenderBuffer();

		void Bind(void) const;
		static void Unbind(void);

		inline uint32_t	GetID(void) const
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
		inline TextureFormat	GetFormatType(void) const
		{
			return m_formatType;
		}
		inline TextureChannelType GetChannelType(void) const
		{
			return m_channelType;
		}
		inline TexturePixelType	GetPixelType(void) const
		{
			return m_pixelType;
		}
		inline int			GetChannelCount(void) const
		{
			return m_channelCount;
		}
	};

}