// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		int Width, int Height,
		Format_Type FormatType,
		Channel_Type m_channelType,
		Pixel_Type PixelType
	)
		: BaseTexture(Texture_Type::TEX_2D, Wrap_Mode::CLAMP_STRETCH, Min_Filter::NEAREST, Mag_Filter::NEAREST, FormatType, m_channelType, PixelType, Anisotropic_Mode::NONE, false)
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