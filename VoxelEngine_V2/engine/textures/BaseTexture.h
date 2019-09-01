// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../rendering/Graphics.h"
#include "../utilities/Asset.h"
#include "../math/Color.h"

#include <map>
#include <string>

namespace Vxl
{
	class RenderBuffer;
	
	void FlipTextureY(UCHAR* array, uint32_t width, uint32_t height, uint32_t channels);

	class BaseTexture
	{
	protected:
		// Data //
		TextureID	 m_id = -1;
		bool		 m_loaded = false;
		bool		 m_mipMapping = false;
		int			 m_width;
		int			 m_height;
		int			 m_channelCount;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);
		TextureType			m_type;
		TextureWrapping		m_wrapMode;
		TextureFilter		m_filterMode;
		TextureFormat		m_formatType;
		TextureChannelType	m_channelType;
		TexturePixelType	m_pixelType;
		AnisotropicMode		m_anisotropicMode;

		// Utility
		void updateParameters();
		void updateStorage();
		void updateStorage(const void* pixels);
		void updateTexImageCubemap(unsigned int side = 0, const void* pixels = nullptr);
		void updateMipmapping();
		void FlipImageVertically(uint8_t* imagePixels);

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			TextureType Type,
			TextureWrapping WrapMode = TextureWrapping::REPEAT,
			TextureFilter FilterMode = TextureFilter::LINEAR,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TextureChannelType ChannelType = TextureChannelType::RGBA,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode AnisotropicMode = AnisotropicMode::NONE,
			bool MipMapping = false
		);
		virtual ~BaseTexture();

		void Bind(TextureLevel layer) const;
		void Bind() const;
		void Unbind() const;

		void Copy(const BaseTexture& _texture);
		void Copy(const RenderBuffer& _texture);

		void setWrapMode(TextureWrapping W);
		void setFilterMode(TextureFilter filter);
		void setAnistropicMode(AnisotropicMode Anso);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		inline TextureID	GetID(void) const
		{
			return m_id;
		}
		inline bool			IsMipMapping(void) const
		{
			return m_mipMapping;
		}
		inline int			GetWidth(void) const
		{
			return m_width;
		}
		inline int			GetHeight(void) const
		{
			return m_height;
		}
		inline int			GetChannelCount(void) const
		{
			return m_channelCount;
		}
		inline Color4F		GetBorderColor(void) const
		{
			return m_borderColor;
		}
		inline TextureType			GetType(void) const
		{
			return m_type;
		}
		inline TextureWrapping		GetWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline TextureFilter		GetFilterMode(void) const
		{
			return m_filterMode;
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

		virtual bool IsLoaded(void) const
		{
			return true;
		}
	};
}