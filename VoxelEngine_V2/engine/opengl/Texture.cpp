// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include <assert.h>

#include "glUtil.h"
#include "../utilities/logger.h"

namespace Vxl
{
	Database<Texture> Texture::m_database;

	/* BASE TEXTURE */

	BaseTexture::BaseTexture(
		Wrap_Mode WrapMode,
		Filter_Mode MinFilter,
		Filter_Mode MaxFilter
	) : m_minFilter(MinFilter), m_maxFilter(MaxFilter)
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
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
	void BaseTexture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void BaseTexture::updateParameters()
	{
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)m_wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)m_wrapMode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)m_minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)m_maxFilter);

		if (m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));
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

	/* TEXTURE */

	Texture::Texture(const std::string& filePath, Wrap_Mode WrapMode, Filter_Mode MinFilter, Filter_Mode MaxFilter)
		: BaseTexture(WrapMode, MinFilter, MaxFilter)
	{
		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			glDeleteTextures(1, &m_id);
			return;
		}

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

	/* RENDER TEXTURE */

	RenderTexture::RenderTexture(int Width, int Height, Format_Type Format, Wrap_Mode WrapMode, Filter_Mode MinFilter, Filter_Mode MaxFilter)
		: BaseTexture(WrapMode, MinFilter, MaxFilter), m_format(Format)
	{
		m_width = Width;
		m_height = Height;

		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)Format, m_width, m_height, 0, (GLenum)Format, GL_UNSIGNED_BYTE, NULL);
	
		Unbind();
	}

}