// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		int Width, int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
		: BaseTexture(TextureType::TEX_2D, TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			FormatType, Graphics::GetChannelType(FormatType), PixelType, AnisotropicMode::NONE, false)
	{
		m_width = Width;
		m_height = Height;

		// Storage
		updateStorage();
		Unbind();
	}

	// Update mipmaps
	void RenderTexture::updateMipmapping()
	{
		Bind();
		m_mipMapping = true;
		BaseTexture::updateMipmapping();
	}
}