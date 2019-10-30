// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Asset.h"

#include "../textures/Texture2D.h"
#include "../textures/Cubemap.h"
#include "../utilities/FileIO.h"

namespace Vxl
{
	Texture2DIndex Assets::loadTexture2D(
		const std::string& filePath,
		bool				InvertY,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New Data
		Texture2D* _texture = new Texture2D(filePath, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture);
	}
	Texture2DIndex Assets::createTexture2D(
		std::vector<Color3F> pixels, uint32_t width,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture);
	}
	Texture2DIndex Assets::createTexture2D(
		void* pixels, uint32_t width, uint32_t height,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, height, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture);
	}
	CubemapIndex Assets::loadCubemap(
		const std::string& filePath1, const std::string& filePath2,
		const std::string& filePath3, const std::string& filePath4,
		const std::string& filePath5, const std::string& filePath6,
		bool InvertY,
		bool UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New DAta
		Cubemap* _cubemap = new Cubemap(filePath1, filePath2, filePath3, filePath4, filePath5, filePath6, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_cubemap_storage.Add(_cubemap);
	}
	void Assets::loadFile(
		const std::string& name,
		const std::string& filepath
	)
	{
		// Create New DAta
		File* _file = new File(filepath);
		// Store Data and Return index
		m_file_storage.Add(stringUtil::toLowerCopy(name), _file);
	}
}