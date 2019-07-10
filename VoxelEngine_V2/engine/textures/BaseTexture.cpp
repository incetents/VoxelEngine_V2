// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "BaseTexture.h"

#include "../rendering/Graphics.h"
#include "../utilities/Macros.h"

#include <assert.h>

namespace Vxl
{
	// Keep track of all active texture slots
	//std::map<TextureType, UINT> BaseTexture::m_activeTextures;

	void FlipTextureY(UCHAR* array, uint32_t width, uint32_t height, uint32_t channels)
	{
		unsigned char* Tmp = new unsigned char[width * channels];
		for (uint32_t y = 0; y < height / 2; y++)
		{
			// Top row
			UINT index1 = ((height - y) - 1) * width * channels;
			UINT index2 = y * width * channels;
			// Temp
			memcpy(Tmp, &array[index1], width * channels);
			// Swap
			memcpy(&array[index1], &array[index2], width * channels);
			memcpy(&array[index2], Tmp, width * channels);
		}
	}

	/* BASE TEXTURE */

	void BaseTexture::updateParameters()
	{
		Graphics::Texture::Bind(m_type, m_id);

		Graphics::Texture::SetWrapping(m_type, m_wrapMode);
		Graphics::Texture::SetFiltering(m_type, m_filterMode, m_mipMapping);

		if (m_wrapMode == TextureWrapping::CLAMP_BORDER)
			Graphics::Texture::SetBorderColor(m_type, m_borderColor);
		
		Graphics::Texture::SetAnistropic(m_type, m_anisotropicMode);
		
	}
	void BaseTexture::updateStorage()
	{
		Graphics::Texture::SetStorage(m_type, m_mipMapping ? 3 : 1, m_formatType, m_width, m_height);
	}
	void BaseTexture::updateStorage(const void* pixels)
	{
		Graphics::Texture::SetStorage(m_type, m_mipMapping ? 3 : 1, m_formatType, m_width, m_height,
			pixels, m_channelType, m_pixelType);
	
	}
	void BaseTexture::updateTexImageCubemap(unsigned int side, const void* pixels)
	{
		Graphics::Texture::SetStorageCubemap(side, 1, m_formatType, m_width, m_height,
			pixels, m_channelType, m_pixelType);
	}
	void BaseTexture::updateMipmapping()
	{
		if (m_mipMapping)
			Graphics::Texture::GenerateMipmap(m_type);
	}

	BaseTexture::BaseTexture(
		TextureType Type,
		TextureWrapping WrapMode,
		TextureFilter FilterMode,
		TextureFormat FormatType,
		TextureChannelType ChannelType,
		TexturePixelType PixelType,
		AnisotropicMode AnisotropicMode,
		bool MipMapping
	)
		: m_type(Type),
		m_wrapMode(WrapMode),
		m_filterMode(FilterMode),
		m_formatType(FormatType),
		m_channelType(ChannelType),
		m_pixelType(PixelType),
		m_anisotropicMode(AnisotropicMode),
		m_mipMapping(MipMapping)
	{
		m_id = Graphics::Texture::Create();

		m_channelCount = Graphics::GetChannelCount(ChannelType);
		if (m_channelCount == -1)
			m_channelCount = Graphics::GetChannelCount(FormatType);

		updateParameters();
	}
	BaseTexture::~BaseTexture()
	{
		Graphics::Texture::Delete(m_id);
	}

	void BaseTexture::Bind(TextureLevel layer) const
	{
		Graphics::Texture::SetActiveLevel(layer);
		Graphics::Texture::Bind(m_type, m_id);
	}
	void BaseTexture::Bind() const
	{
		Graphics::Texture::Bind(m_type, m_id);
	}
	void BaseTexture::Unbind() const
	{
		Graphics::Texture::Unbind(m_type);
	}

	void BaseTexture::setWrapMode(TextureWrapping W)
	{
		m_wrapMode = W;

		updateParameters();
	}
	void BaseTexture::setFilterMode(TextureFilter filter)
	{
		m_filterMode = filter;

		updateParameters();
	}
	void BaseTexture::setAnistropicMode(AnisotropicMode Anso)
	{
		m_anisotropicMode = Anso;

		updateParameters();
	}
	void BaseTexture::setBorderColor(Color4F color)
	{
		m_borderColor = color;

		updateParameters();
	}
}