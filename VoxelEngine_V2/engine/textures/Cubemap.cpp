// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Cubemap.h"

#include "../rendering/Graphics.h"

#include "../utilities/stringUtil.h"

#include <SOIL/SOIL.h>

namespace Vxl
{
	Cubemap::Cubemap(
		const std::string& filePath1,
		const std::string& filePath2,
		const std::string& filePath3,
		const std::string& filePath4,
		const std::string& filePath5,
		const std::string& filePath6,
		bool			InvertY,
		bool			UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
		: BaseTexture(TextureType::TEX_CUBEMAP, WrapMode, FilterMode, FormatType, TextureChannelType::NONE, PixelType, AnisotropicMode, UseMipMapping)
	{
		m_image[0] = SOIL_load_image(filePath1.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);
		m_image[1] = SOIL_load_image(filePath2.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);
		m_image[2] = SOIL_load_image(filePath3.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);
		m_image[3] = SOIL_load_image(filePath4.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);
		m_image[4] = SOIL_load_image(filePath5.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);
		m_image[5] = SOIL_load_image(filePath6.c_str(), &m_width, &m_height, &m_channelCount, SOIL_LOAD_AUTO);

		for (uint32_t i = 0; i < 6; i++)
		{
			if (!m_image[i])
				return;

			else if (InvertY)
				flipImageVertically(m_image[i]);
		}

		m_channelType = Graphics::GetChannelType(m_channelCount);

		createStorage();

		// Storage
		for (uint32_t i = 0; i < 6; i++)
		{
			SetStorageCubemap((CubemapFace)(i + 1), m_image[i]);
		}
		updateMipmapping();

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath1);
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "Cubemap_" + Name);

		// finished
		unbind();
		m_loaded = true;
	}
	Cubemap::~Cubemap()
	{
		for (uint32_t i = 0; i < 6; i++)
			delete[] m_image[i];
	}

	void Cubemap::SetStorageCubemap(CubemapFace face, const void* pixels)
	{
		Graphics::Texture::SetStorage(face, m_width, m_height, m_channelType, m_pixelType, pixels);
	}

}