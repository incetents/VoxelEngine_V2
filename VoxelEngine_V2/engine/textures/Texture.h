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
			const std::string& filePath,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Min_Filter		MinFilter = Min_Filter::LINEAR,
			Mag_Filter		MagFilter = Mag_Filter::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Channel_Type	ChannelType = Channel_Type::RGBA,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE
		);
		// Constructor [Create custom]
		Texture(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Min_Filter		MinFilter = Min_Filter::LINEAR,
			Mag_Filter		MagFilter = Mag_Filter::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Channel_Type	ChannelType = Channel_Type::RGBA,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE
		);

	public:
		// Load Texture
		static Texture* Load(
			const std::string& name,
			const std::string& filePath,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Min_Filter		MinFilter = Min_Filter::LINEAR,
			Mag_Filter		MagFilter = Mag_Filter::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Channel_Type	ChannelType = Channel_Type::RGBA,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE
		);
		// Create custom texture
		static Texture* CreateCustom(
			const std::string& name,
			std::vector<Color3F> pixels, UINT width,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Min_Filter		MinFilter = Min_Filter::LINEAR,
			Mag_Filter		MagFilter = Mag_Filter::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Channel_Type	ChannelType = Channel_Type::RGBA,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE
		);
		

		Texture(const Texture&) = delete;
		~Texture();

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};
}

