// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Asset.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../textures/Cubemap.h"
#include "../utilities/FileIO.h"
#include "../rendering/RenderBuffer.h"

namespace Vxl
{
	void Assets::DestroyAndEraseAll()
	{
		// Delete All Assets
		auto& textures = getAllTexture2D();
		for (auto& texture : textures)
			delete texture.second;

		auto& cubemaps = getAllCubemap();
		for (auto& cubemap : cubemaps)
			delete cubemap.second;

		auto& files = getAllFiles();
		for (auto& file : files)
			delete file.second;

		auto& renderTexture = getAllRenderTexture();
		for (auto& rt : renderTexture)
			delete rt.second;

		auto& renderTextureDepth = getAllRenderTextureDepth();
		for (auto& rtd : renderTextureDepth)
			delete rtd.second;

		auto& renderBuffer = getAllRenderBuffer();
		for (auto& rb : renderBuffer)
			delete rb.second;

		auto& renderBufferDepth = getAllRenderBufferDepth();
		for (auto& rbd : renderBufferDepth)
			delete rbd.second;

		// erase All Assets
		m_texture2D_storage.EraseAll();
		m_cubemap_storage.EraseAll();
		m_file_storage.EraseAll();
		m_renderTexture_storage.EraseAll();
		m_renderTextureDepth_storage.EraseAll();
		m_renderBuffer_storage.EraseAll();
		m_renderBufferDepth_storage.EraseAll();
	}

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
		// Create New Data
		File* _file = new File(filepath);
		// Store Data and Return index
		m_file_storage.Add(stringUtil::toLowerCopy(name), _file);
	}
	RenderTextureIndex Assets::createRenderTexture(
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType,
		bool MipMapping
	)
	{
		// Create New Data
		RenderTexture* _texture = new RenderTexture(Width, Height, FormatType, PixelType, MipMapping);
		// Store Data and Return index
		return m_renderTexture_storage.Add(_texture);
	}
	RenderTextureDepthIndex Assets::createRenderTextureDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
	{
		// Create New Data
		RenderTextureDepth* _texture = new RenderTextureDepth(Width, Height, FormatType);
		// Store Data and Return index
		return m_renderTextureDepth_storage.Add(_texture);
	}
	RenderBufferIndex Assets::createRenderBuffer(
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
	{
		// Create New Data
		RenderBuffer* _buffer = new RenderBuffer(Width, Height, FormatType, PixelType);
		// Store Data and Return index
		return m_renderBuffer_storage.Add(_buffer);
	}
	RenderBufferDepthIndex Assets::createRenderBufferDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
	{
		// Create New Data
		RenderBufferDepth* _buffer = new RenderBufferDepth(Width, Height, FormatType);
		// Store Data and Return index
		return m_renderBufferDepth_storage.Add(_buffer);
	}
}