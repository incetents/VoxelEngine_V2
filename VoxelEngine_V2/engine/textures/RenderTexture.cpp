// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		std::string name,
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
		: m_name(name), BaseTexture(TextureType::TEX_2D, TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			FormatType, Graphics::GetChannelType(FormatType), PixelType, AnisotropicMode::NONE, false)
	{
		m_width = Width;
		m_height = Height;

		// Storage
		updateStorage();
		Unbind();
	}

	// Asset Creation
	RenderTexture* RenderTexture::Create(
		std::string name,
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
	{
		RenderTexture* _texture = new RenderTexture(name, Width, Height, FormatType, PixelType);

		if(name.empty())
			AddUnnamedAsset(_texture, AssetMessage::CREATED);
		else
			AddNamedAsset(name, _texture, AssetMessage::CREATED);

		if (!_texture->IsLoaded())
		{
			Logger.error("Render Texture failed to create");
			return nullptr;
		}

		return _texture;
	}

	RenderTexture::~RenderTexture()
	{
	}

	// Utility
	void RenderTexture::RecreateStorage(uint32_t width, uint32_t height, TextureFormat format, TexturePixelType pixelType)
	{
		// Texture is immutable, destroy it and create a new one
		Unbind();
		Graphics::Texture::Delete(m_id);
		m_id = Graphics::Texture::Create();
		Bind();

		// Fix values
		m_width = (int)width;
		m_height = (int)height;
		m_formatType = format;
		m_pixelType = pixelType;
		
		// Update gl values
		updateParameters();
		updateStorage();
		Unbind();
	}
	void RenderTexture::updateMipmapping()
	{
		Bind();
		m_mipMapping = true;
		BaseTexture::updateMipmapping();
	}
}