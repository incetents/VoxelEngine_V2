// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include <assert.h>

#include "glUtil.h"
#include "../utilities/logger.h"

namespace Vxl
{
	Texture::Texture(
		const std::string& filePath,
		Wrap_Mode WrapMode,
		Filter_Mode MinFilter,
		Filter_Mode MaxFilter
	)
		: m_wrapMode(WrapMode), m_maxFilter(MaxFilter), m_minFilter(MinFilter)
	{
		assert(m_maxFilter == Filter_Mode::LINEAR || m_maxFilter == Filter_Mode::NEAREST);

		glGenTextures(1, &m_id);

		update();

		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			glDeleteTextures(1, &m_id);
			return;
		}

		GLenum Format = (GLenum)glUtil::getFormatFloat(m_channels);

		glTexImage2D(GL_TEXTURE_2D, 0, Format, m_width, m_height, 0, Format, GL_UNSIGNED_BYTE, m_image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture::update()
	{
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)m_wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)m_wrapMode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)m_minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)m_maxFilter);

		if(m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));
	}

	Texture::~Texture()
	{
		delete[] m_image;
		glDeleteTextures(1, &m_id);
	}

	void Texture::setWrapMode(Wrap_Mode W)
	{
		m_wrapMode = W;

		update();
	}
	void Texture::setFilterMode(Filter_Mode Min, Filter_Mode Max)
	{
		m_minFilter = Min;
		m_maxFilter = Max;
		assert(m_maxFilter == Filter_Mode::LINEAR || m_maxFilter == Filter_Mode::NEAREST);

		update();
	}
	// Only works for 
	void Texture::setBorderColor(Color4F color)
	{
		m_borderColor = color;

		update();
	}

	void Texture::bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
	void Texture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}