// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"
#include "../utilities/Types.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	class BaseTexture;

	class RenderBuffer
	{
		DISALLOW_COPY_AND_ASSIGN(RenderBuffer);
		friend class Assets;
	protected:
		// Data
		uint32_t			m_id = -1;
		int					m_width;
		int					m_height;
		TextureFormat		m_formatType;
		TextureChannelType	m_channelType;
		TexturePixelType	m_pixelType;
		int					m_channelCount;

		RenderBuffer(
			int Width,
			int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);
		void UpdateStorage();

	public:
		~RenderBuffer();

		void RecreateStorage(uint32_t width, uint32_t height, TextureFormat format);
		void setGLName(const std::string& name);

		void Bind(void) const;
		static void Unbind(void);
		
		void Copy(const BaseTexture& _texture);
		void Copy(const RenderBuffer& _texture);

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

	};

	class RenderBufferDepth : public RenderBuffer
	{
		DISALLOW_COPY_AND_ASSIGN(RenderBufferDepth);
		friend class Assets;
	protected:
		// Created using only depth format
		RenderBufferDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);

	public:
		// Additional Data
		const TextureDepthFormat m_depthFormat;
	};
}