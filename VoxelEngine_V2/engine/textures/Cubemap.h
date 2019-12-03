// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class Cubemap : public BaseTexture
	{
		DISALLOW_COPY_AND_ASSIGN(Cubemap);
		friend class _Assets;
	protected:
		UCHAR**		m_image = new UCHAR*[6];

		// Constructor
		Cubemap(
			const std::string& filePath1,
			const std::string& filePath2,
			const std::string& filePath3,
			const std::string& filePath4,
			const std::string& filePath5,
			const std::string& filePath6,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
	public:

		~Cubemap();

		void SetStorageCubemap(CubemapFace face, const void* pixels);

		inline bool isLoaded(void) const override
		{
			return m_loaded;
		}
	};
}