// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Cubemap.h"

#include "../opengl/glUtil.h"
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
		Wrap_Mode		WrapMode,
		Min_Filter MinFilter,
		Mag_Filter MagFilter,
		Format_Type		FormatType,
		Pixel_Type		PixelType
	)
		: BaseTexture(Texture_Type::TEX_CUBEMAP, WrapMode, MinFilter, MagFilter, FormatType, Channel_Type::NONE, PixelType)
	{
		m_useMipMapping = UseMipMapping;

		m_image[0] = SOIL_load_image(filePath1.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[1] = SOIL_load_image(filePath2.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[2] = SOIL_load_image(filePath3.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[3] = SOIL_load_image(filePath4.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[4] = SOIL_load_image(filePath5.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[5] = SOIL_load_image(filePath6.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		for (GLuint i = 0; i < 6; i++)
		{
			if (!m_image[i])
				return;
			else if (InvertY)
				FlipTextureY(m_image[i], m_width, m_height, m_channels);
		}

		m_channelType = glUtil::getFormat(m_channels);

		for (GLuint i = 0; i < 6; i++)
		{
			updateTexImageCubemap(i, m_image[i]);
		}

		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath1);
		glUtil::setGLName(glNameType::TEXTURE, m_id, "Cubemap_" + Name);

		// finished
		Unbind();
		m_loaded = true;
	}
	Cubemap::~Cubemap()
	{
		for (GLuint i = 0; i < 6; i++)
			delete[] m_image[i];
	}

	Cubemap* Cubemap::Load(
		const std::string& name,
		const std::string& filePath1, const std::string& filePath2, const std::string& filePath3, const std::string& filePath4, const std::string& filePath5, const std::string& filePath6,
		bool InvertY,
		bool UseMipMapping,
		Wrap_Mode WrapMode,
		Min_Filter MinFilter,
		Mag_Filter MagFilter,
		Format_Type	FormatType,
		Pixel_Type PixelType
	) {
		Cubemap* _cubemap = new Cubemap(filePath1, filePath2, filePath3, filePath4, filePath5, filePath6, InvertY, UseMipMapping, WrapMode, MinFilter, MagFilter, FormatType, PixelType);

		AddToDatabase(name, _cubemap);

		if (_cubemap == nullptr)
			return false;
		else if (!_cubemap->IsLoaded())
			return false;

		Message_Loaded(name, _cubemap);

		return _cubemap;
	}
}