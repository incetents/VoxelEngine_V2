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
		friend class _Assets;
	protected:
		// Data
		std::string			m_name;
		uint32_t			m_id = -1;
		int					m_width;
		int					m_height;
		TextureFormat		m_formatType;
		TextureChannelType	m_channelType;
		TexturePixelType	m_pixelType;
		int					m_channelCount;

		RenderBuffer(
			const std::string& name,
			int Width,
			int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);
		void UpdateStorage();

	public:
		virtual ~RenderBuffer();

		void RecreateStorage(uint32_t width, uint32_t height, TextureFormat format);
		void setGLName(const std::string& name);

		void bind(void) const;
		static void Unbind(void);
		
		//void Copy(const BaseTexture& _texture);
		//void Copy(const RenderBuffer& _texture);

		// Getters
		inline std::string getName(void) const
		{
			return m_name;
		}
		inline uint32_t	getID(void) const
		{
			return m_id;
		}
		inline int		getWidth(void) const
		{
			return m_width;
		}
		inline int		getHeight(void) const
		{
			return m_height;
		}
		inline TextureFormat		getFormatType(void) const
		{
			return m_formatType;
		}
		inline TextureChannelType	getChannelType(void) const
		{
			return m_channelType;
		}
		inline TexturePixelType		getPixelType(void) const
		{
			return m_pixelType;
		}
		inline int					getChannelCount(void) const
		{
			return m_channelCount;
		}

	};

	class RenderBufferDepth : public RenderBuffer
	{
		DISALLOW_COPY_AND_ASSIGN(RenderBufferDepth);
		friend class _Assets;
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