// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include <assert.h>

#include "glUtil.h"
#include "../utilities/logger.h"

namespace Vxl
{
	void FlipTextureY(UCHAR* array, GLuint width, GLuint height, GLuint channels)
	{
		unsigned char* Tmp = new unsigned char[width * channels];
		for (GLuint y = 0; y < height / 2; y++)
		{
			// Top row
			UINT index1 = ((height - y) - 1) * width * channels;
			UINT index2 = y * width * channels;
			// Temp
			memcpy(Tmp, &array[index1], width * channels);
			// Swap
			memcpy(&array[index1], &array[index2], width * channels);
			memcpy(&array[index2], Tmp, width * channels);
		}
	}

	// Databases
	Database<Texture> Texture::m_database;
	Database<Cubemap> Cubemap::m_database;

	/* BASE TEXTURE */

	void BaseTexture::updateParameters()
	{
		GLenum Target = (GLenum)m_type;

		glBindTexture(Target, m_id);

		glTexParameteri(Target, GL_TEXTURE_WRAP_S, (GLenum)m_wrapMode);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, (GLenum)m_wrapMode);

		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, (GLenum)m_minFilter);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, (GLenum)m_maxFilter);

		if (m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));
	}

	BaseTexture::BaseTexture(
		Texture_Type Type,
		Wrap_Mode WrapMode,
		Filter_Mode MinFilter,
		Filter_Mode MaxFilter
	) : m_type(Type), m_minFilter(MinFilter), m_maxFilter(MaxFilter)
	{
		assert(m_maxFilter == Filter_Mode::LINEAR || m_maxFilter == Filter_Mode::NEAREST);

		glGenTextures(1, &m_id);

		updateParameters();
	}
	BaseTexture::~BaseTexture()
	{
		if(m_id != -1)
			glDeleteTextures(1, &m_id);
	}

	void BaseTexture::Bind()
	{
		glBindTexture((GLenum)m_type, m_id);
	}
	void BaseTexture::Unbind()
	{
		glBindTexture(0, m_id);
	}

	void BaseTexture::setWrapMode(Wrap_Mode W)
	{
		m_wrapMode = W;

		updateParameters();
	}
	void BaseTexture::setFilterMode(Filter_Mode Min, Filter_Mode Max)
	{
		m_minFilter = Min;
		m_maxFilter = Max;
		assert(m_maxFilter == Filter_Mode::LINEAR || m_maxFilter == Filter_Mode::NEAREST);

		updateParameters();
	}
	void BaseTexture::setBorderColor(Color4F color)
	{
		m_borderColor = color;

		updateParameters();
	}

	/* RENDER TEXTURE */

	RenderTexture::RenderTexture(int Width, int Height, Format_Type Format, Wrap_Mode WrapMode, Filter_Mode MinFilter, Filter_Mode MaxFilter)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MaxFilter), m_format(Format)
	{
		m_width = Width;
		m_height = Height;

		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)Format, m_width, m_height, 0, (GLenum)Format, GL_UNSIGNED_BYTE, NULL);

		Unbind();
	}

	/* TEXTURE */

	Texture::Texture(const std::string& filePath, Wrap_Mode WrapMode, Filter_Mode MinFilter, Filter_Mode MaxFilter, bool InvertY)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MaxFilter)
	{
		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			Logger.error("SOIL: " + std::string(SOIL_last_result()));
			glDeleteTextures(1, &m_id);
			return;
		}

		// Invert Y
		if (InvertY)
			FlipTextureY(m_image, m_width, m_height, m_channels);
		//

		m_loaded = true;

		GLenum Format = (GLenum)glUtil::getFormatFloat(m_channels);

		glTexImage2D(GL_TEXTURE_2D, 0, Format, m_width, m_height, 0, Format, GL_UNSIGNED_BYTE, m_image);
		glGenerateMipmap(GL_TEXTURE_2D);

		Unbind();
	}

	Texture::~Texture()
	{
		delete[] m_image;
	}

	/* Cubemap */
	Cubemap::Cubemap(
		const std::string& filePath1,
		const std::string& filePath2,
		const std::string& filePath3,
		const std::string& filePath4,
		const std::string& filePath5,
		const std::string& filePath6,
		Wrap_Mode WrapMode,
		Filter_Mode MinFilter,
		Filter_Mode MaxFilter,
		bool InvertY
	) : BaseTexture(Texture_Type::TEX_CUBEMAP, WrapMode, MinFilter, MaxFilter)
	{
		m_image[0] = SOIL_load_image(filePath1.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[1] = SOIL_load_image(filePath2.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[2] = SOIL_load_image(filePath3.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[3] = SOIL_load_image(filePath4.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[4] = SOIL_load_image(filePath5.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
		m_image[5] = SOIL_load_image(filePath6.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		for (GLuint i = 0; i < 6; i++)
			if (!m_image[i])
				return;

		m_loaded = true;

		GLenum Format = (GLenum)glUtil::getFormatFloat(m_channels);

		for (GLuint i = 0; i < 6; i++)
		{
			//FlipTextureY(m_image[i], m_width, m_height, m_channels);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, Format, m_width, m_height, 0, Format, GL_UNSIGNED_BYTE, m_image[i]
			);
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		Unbind();
	}
	Cubemap::~Cubemap()
	{
		for (GLuint i = 0; i < 6; i++)
			delete[] m_image[i];
	}

}