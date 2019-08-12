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
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

		// Update mipmaps
		void updateMipmapping();
	};
}