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

	// Update mipmaps
	void RenderTexture::updateMipmapping()
	{
		Bind();
		m_mipMapping = true;
		BaseTexture::updateMipmapping();
	}
}