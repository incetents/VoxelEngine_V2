// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "BaseTexture.h"

#include "../rendering/Graphics.h"
#include "../rendering/RenderBuffer.h"
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
	void BaseTexture::FlipImageVertically(uint8_t* imagePixels)
	{
		VXL_ASSERT(imagePixels, "Cannot flip texture/image if pixels are not stored");

		unsigned char* Tmp = new unsigned char[m_width * m_channelCount];
		for (uint32_t y = 0; y < m_height / 2; y++)
		{
			// Top row
			UINT index1 = ((m_height - y) - 1) * m_width * m_channelCount;
			UINT index2 = y * m_width * m_channelCount;
			// Temp
			memcpy(Tmp, &imagePixels[index1], m_width * m_channelCount);
			// Swap
			memcpy(&imagePixels[index1], &imagePixels[index2], m_width * m_channelCount);
			memcpy(&imagePixels[index2], Tmp, m_width * m_channelCount);
		}
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

	void BaseTexture::Copy(const BaseTexture& _texture)
	{
		// Sizes and Formats must be identical
		VXL_ASSERT((int)m_width == (int)_texture.m_width, "Copy Texture Error [Widths do not match]");
		VXL_ASSERT((int)m_height == (int)_texture.m_height, "Copy Texture Error [Heights do not match]");
		VXL_ASSERT(m_formatType == _texture.m_formatType, "Copy Texture Error [Formats do not match]");

		Graphics::CopyTexture(
			m_id, m_type,
			_texture.m_id, _texture.m_type,
			m_width, m_height
		);
	}
	void BaseTexture::Copy(const RenderBuffer& _texture)
	{
		// Sizes and Formats must be identical
		VXL_ASSERT((int)m_width == (int)_texture.GetWidth(), "Copy Texture Error [Widths do not match]");
		VXL_ASSERT((int)m_height == (int)_texture.GetHeight(), "Copy Texture Error [Heights do not match]");
		VXL_ASSERT(m_formatType == _texture.GetFormatType(), "Copy Texture Error [Formats do not match]");

		Graphics::CopyTexture(
			m_id, m_type,
			_texture.GetID(), TextureType::RENDERBUFFER,
			m_width, m_height
		);
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