// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		int Width, int Height,
		Format_Type FormatType
	)
		: BaseTexture(Texture_Type::TEX_2D, Wrap_Mode::CLAMP_STRETCH, Min_Filter::NEAREST, Mag_Filter::NEAREST, FormatType, Channel_Type::RGBA, Pixel_Type::UNSIGNED_BYTE)
	{
		m_width = Width;
		m_height = Height;

		updateStorage();
		Unbind();
	}
}