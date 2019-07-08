// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class Color3F;

	class Texture : public BaseTexture, public Asset<Texture>
	{
		friend class Loader;
		friend class RenderManager;
	protected:
		UCHAR*		m_image = nullptr;

		// Constructor [Load from file]
		Texture(
			const std::string&	filePath,
			bool				InvertY = true,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			//TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnistropicMode = AnisotropicMode::NONE
		);
		// Constructor [Create custom]
		Texture(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);

	public:
		// Load Texture
		static Texture* Load(
			const std::string& name,
			const std::string& filePath,
			bool				InvertY = true,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			//TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
		// Create custom texture
		static Texture* CreateCustom(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
		

		Texture(const Texture&) = delete;
		~Texture();

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};
}

