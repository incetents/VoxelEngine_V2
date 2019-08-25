// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class Color3F;

	class Texture2D : public BaseTexture, public Asset<Texture2D>
	{
		friend class Loader;
		friend class RenderManager;
	protected:
		uint8_t* m_image = nullptr;

		// Constructor [Load from file]
		Texture2D(
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
		Texture2D(
			const std::string& name,
			std::vector<Color3F> pixels, uint32_t width,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
		// Constructor [Create custom]
		Texture2D(
			const std::string& name,
			void* pixels,
			uint32_t width, uint32_t height,
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
		static Texture2D* Load(
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
		static Texture2D* CreateCustom(
			const std::string& name,
			std::vector<Color3F> pixels, uint32_t width,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
		// Create custom texture
		static Texture2D* CreateCustom(
			const std::string& name,
			void* pixels, uint32_t width, uint32_t height,
			bool				UseMipMapping = true,
			TextureWrapping		WrapMode = TextureWrapping::REPEAT,
			TextureFilter		FilterMode = TextureFilter::LINEAR,
			TextureFormat		FormatType = TextureFormat::RGBA8,
			TextureChannelType	ChannelType = TextureChannelType::RGBA,
			TexturePixelType	PixelType = TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode		AnisotropicMode = AnisotropicMode::NONE
		);
		// Export Texture
		static void ScreenshotBMP(
			const std::string& filePath
		);
		

		Texture2D(const Texture2D&) = delete;
		~Texture2D();

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
		// Returns nullptr if nothing is being stored
		inline uint8_t* GetImageData(void) const
		{
			return m_image;
		}
	};
}

