// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class Cubemap : public BaseTexture, public Asset<Cubemap>
	{
		friend class Loader;
		friend class RenderManager;
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

		// Load Cubemap
		static Cubemap* Load(
			const std::string& name,
			const std::string& filePath1, const std::string& filePath2, const std::string& filePath3, const std::string& filePath4, const std::string& filePath5, const std::string& filePath6,
			bool InvertY = false,
			bool UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::CLAMP_STRETCH,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);

		Cubemap(const Cubemap&) = delete;
		~Cubemap();

		void SetStorageCubemap(CubemapFace face, const void* pixels);

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};
}