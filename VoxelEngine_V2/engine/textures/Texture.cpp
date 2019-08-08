// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include "../rendering/Graphics.h"

#include "../rendering/Graphics.h"
#include "../utilities/logger.h"
#include "../utilities/stringUtil.h"
#include "../math/Color.h"

#include <SOIL/SOIL.h>
#include <assert.h>

namespace Vxl
{
	// [ Loading ]
	Texture::Texture(
		const std::string& filePath,
		bool			InvertY,
		bool			UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		//TextureChannelType	ChannelType,
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
			FlipTextureY(m_image, m_width, m_height, m_channelCount);

		// Storage
		updateStorage(&m_image[0]);
		updateMipmapping();

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath);
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "Tex_" + Name);

		// finished
		Unbind();
		m_loaded = true;
	}
	// [ Create Custom ]
	Texture::Texture(
		const std::string& name,
		std::vector<Color3F> pixels, uint32_t width,
		bool			UseMipMapping,
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

		updateStorage(&m_image[0]);
		updateMipmapping();

		// glName
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "Tex_" + name);

		// finished
		Unbind();
		m_loaded = true;
	}
	// Constructor [Create custom]
	Texture::Texture(
		const std::string& name,
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
		m_image = new UCHAR[pixelCount];
		memcpy(m_image, pixels, pixelCount * sizeof(UCHAR));

		//m_image = (UCHAR*)pixels;

		//updateParameters();
		updateStorage(pixels);
		updateMipmapping();

		// glName
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "Tex_" + name);

		// finished
		Unbind();
		m_loaded = true;
	}

	Texture* Texture::Load(
		const std::string& name,
		const std::string& filePath,
		bool			InvertY,
		bool			UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		//TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	) {
		Texture* _texture = new Texture(filePath, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);

		AddToDatabase(name, _texture);

		if (_texture == nullptr)
			return false;
		else if (!_texture->IsLoaded())
			return false;

		Message_Loaded(name, _texture);

		return _texture;
	}

	Texture* Texture::CreateCustom(
		const std::string& name,
		std::vector<Color3F> pixels, uint32_t width,
		bool			UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	) {
		Texture* _texture = new Texture(name, pixels, width, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);

		AddToDatabase(name, _texture);

		if (_texture == nullptr)
			return false;
		else if (!_texture->IsLoaded())
			return false;

		Message_Created(name, _texture);

		return _texture;
	}
	// Create custom texture
	Texture* Texture::CreateCustom(
		const std::string& name,
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
		Texture* _texture = new Texture(name, pixels, width, height, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);

		AddToDatabase(name, _texture);

		if (_texture == nullptr)
			return false;
		else if (!_texture->IsLoaded())
			return false;

		Message_Created(name, _texture);

		return _texture;
	}

	Texture::~Texture()
	{
		delete[] m_image;
	}

	
}