// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture2D.h"

#include "../rendering/Graphics.h"

#include "../rendering/Graphics.h"
#include "../utilities/Logger.h"
#include "../utilities/stringUtil.h"
#include "../math/Color.h"
#include "../window/window.h"
#include "../utilities/FileIO.h"

#include <SOIL/SOIL.h>

namespace Vxl
{
	// [ Loading ]
	Texture2D::Texture2D(
		const std::string& filePath,
		bool			InvertY,
		bool			UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
		: BaseTexture(TextureType::TEX_2D, WrapMode, FilterMode, FormatType, TextureChannelType::NONE, PixelType, AnisotropicMode, UseMipMapping)
	{
		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			Logger.error("SOIL: " + std::string(SOIL_last_result()));
			return;
		}

		// Get Correct channel data
		m_channelType = Graphics::GetChannelType(m_channelCount);

		// Invert Y
		if (InvertY)
			FlipImageVertically(m_image);

		// Storage
		CreateStorage();
		SetStorage(&m_image[0]);
		UpdateMipmapping();

		// glName
		auto name = stringUtil::nameFromFilepath(filePath);
		if (!name.empty())
			setGLName("Tex2D_" + name);

		// finished
		Unbind();
		m_loaded = true;
	}
	// [ Create Custom ]
	Texture2D::Texture2D(
		std::vector<Color3F>pixels,
		uint32_t			width,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	) 
		: BaseTexture(TextureType::TEX_2D, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode, UseMipMapping)
	{

		UINT pixelsCount = (UINT)pixels.size();
		m_width = width;
		m_height = pixelsCount / width;
		m_channelCount = 3;

		std::vector<UCHAR> fixed_pixels;
		fixed_pixels.reserve(pixelsCount * m_channelCount);

		for (UINT i = 0; i < pixelsCount; i++)
		{
			fixed_pixels.push_back((UCHAR)(pixels[i].r * 255.0f));
			fixed_pixels.push_back((UCHAR)(pixels[i].g * 255.0f));
			fixed_pixels.push_back((UCHAR)(pixels[i].b * 255.0f));
		}

		m_image = new UCHAR[pixelsCount * m_channelCount];
		memcpy(m_image, &fixed_pixels[0], fixed_pixels.size() * sizeof(UCHAR));

		// Storage
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_1);

		CreateStorage();
		SetStorage(&m_image[0]);

		UpdateMipmapping();

		// finished
		Unbind();
		m_loaded = true;
	}
	// Constructor [Create custom]
	Texture2D::Texture2D(
		void* pixels,
		uint32_t width, uint32_t height,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)	
		: BaseTexture(TextureType::TEX_2D, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode, UseMipMapping)
	{
		m_width = width;
		m_height = height;
		m_channelCount = Graphics::GetChannelCount(ChannelType);

		// Storage
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_1);

		uint32_t pixelCount = width * height * m_channelCount;
		m_image = new uint8_t[pixelCount];
		memcpy(m_image, pixels, pixelCount * sizeof(uint8_t));

		CreateStorage();
		SetStorage(&m_image[0]);

		UpdateMipmapping();

		// finished
		Unbind();
		m_loaded = true;
	}

	// Export Texture
	void Texture2D::ScreenshotBMP(
		const std::string& filePath
	)
	{
		std::string finalFilePath = filePath + ".bmp";

		FileIO::DuplicateFixer(finalFilePath);
		FileIO::EnsureDirectory(finalFilePath);

		int result = SOIL_save_screenshot
		(
			finalFilePath.c_str(),
			SOIL_SAVE_TYPE_BMP,
			0, 0, Window.GetWindowWidth(), Window.GetWindowHeight()
		);
		VXL_ASSERT(result > 0, "Screenshot Failed");
	}

	Texture2D::~Texture2D()
	{
		delete[] m_image;
	}

	
}