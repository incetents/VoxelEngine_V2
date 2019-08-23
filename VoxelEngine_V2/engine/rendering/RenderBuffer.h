// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	class RenderBuffer : public Asset<RenderBuffer>
	{
	private:
		// Data
		uint32_t			m_id = -1;
		int					m_width;
		int					m_height;
		TextureFormat		m_formatType;
		TextureChannelType	m_channelType;
		TexturePixelType	m_pixelType;
		int					m_channelCount;
		const std::string	m_name;

		RenderBuffer(
			std::string name,
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

	public:
		RenderBuffer(const RenderBuffer&) = delete;

		static RenderBuffer* Create(
			std::string name,
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
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
		inline TextureFormat		GetFormatType(void) const
		{
			return m_formatType;
		}
		inline TextureChannelType	GetChannelType(void) const
		{
			return m_channelType;
		}
		inline TexturePixelType		GetPixelType(void) const
		{
			return m_pixelType;
		}
		inline int					GetChannelCount(void) const
		{
			return m_channelCount;
		}
		inline std::string			GetName(void) const
		{
			return m_name;
		}
	};

}