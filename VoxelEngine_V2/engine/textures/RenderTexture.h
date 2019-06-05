// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class RenderTexture : public BaseTexture
	{
	public:
		RenderTexture(const RenderTexture&) = delete;

		// Channel/Pixel used only for reading
		RenderTexture(
			int Width, int Height,
			Format_Type FormatType = Format_Type::RGBA8,
			Channel_Type m_channelType = Channel_Type::RGBA,
			Pixel_Type PixelType = Pixel_Type::UNSIGNED_BYTE
		);
	};
}