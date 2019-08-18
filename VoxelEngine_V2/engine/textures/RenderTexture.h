// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class RenderTexture : public BaseTexture, public Asset<RenderTexture>
	{
	private:
		// Channel/Pixel used only for reading
		RenderTexture(
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

	public:
		RenderTexture(const RenderTexture&) = delete;

		~RenderTexture();

		// Asset Creation
		static RenderTexture* Create(
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

		// Update mipmaps
		void updateMipmapping();
	};
}