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
			std::string name,
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

		// Data
		std::string m_name;

	public:
		RenderTexture(const RenderTexture&) = delete;

		~RenderTexture();

		// Asset Creation
		static RenderTexture* Create(
			std::string name,
			int Width, int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE
		);

		// Get Name
		inline std::string GetName(void) const
		{
			return m_name;
		}

		// Update mipmaps
		void updateMipmapping();
	};
}