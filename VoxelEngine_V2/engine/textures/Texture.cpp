// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include "../opengl/glUtil.h"
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
		Wrap_Mode		WrapMode,
		Min_Filter		MinFilter,
		Mag_Filter		MagFilter,
		Format_Type		FormatType,
		Channel_Type	ChannelType,
		Pixel_Type		PixelType
	)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, PixelType)
	{
		m_useMipMapping = UseMipMapping;

		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			Logger.error("SOIL: " + std::string(SOIL_last_result()));
			return;
		}

		// Get Correct channel data
		m_channelType = glUtil::getChannelType(m_channelCount);

		// Invert Y
		if (InvertY)
			FlipTextureY(m_image, m_width, m_height, m_channelCount);

		// Storage
		updateStorage(&m_image[0]);

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath);
		glUtil::setGLName(glNameType::TEXTURE, m_id, "Tex_" + Name);

		// finished
		Unbind();
		m_loaded = true;
	}
	// [ Create Custom ]
	Texture::Texture(
		const std::string& name,
		std::vector<Color3F> pixels, UINT width,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Min_Filter		MinFilter,
		Mag_Filter		MagFilter,
		Format_Type		FormatType,
		Channel_Type	ChannelType,
		Pixel_Type		PixelType
	) 
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, PixelType)
	{
		m_useMipMapping = UseMipMapping;

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // helps prevent textures smaller than 4x4 not get corrupted
		updateStorage(&m_image[0]);

		// glName
		glUtil::setGLName(glNameType::TEXTURE, m_id, "Tex_" + name);

		// finished
		Unbind();
		m_loaded = true;
	}

	Texture* Texture::Load(
		const std::string& name,
		const std::string& filePath,
		bool			InvertY,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Min_Filter		MinFilter,
		Mag_Filter		MagFilter,
		Format_Type		FormatType,
		Channel_Type	ChannelType,
		Pixel_Type		PixelType
	) {
		Texture* _texture = new Texture(filePath, InvertY, UseMipMapping, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, PixelType);

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
		std::vector<Color3F> pixels, UINT width,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Min_Filter		MinFilter,
		Mag_Filter		MagFilter,
		Format_Type		FormatType,
		Channel_Type	ChannelType,
		Pixel_Type		PixelType
	) {
		Texture* _texture = new Texture(name, pixels, width, UseMipMapping, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, PixelType);

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