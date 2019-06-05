// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "BaseTexture.h"

#include "../opengl/glUtil.h"

#include <assert.h>

namespace Vxl
{
	// Keep track of all active texture slots
	std::map<Texture_Type, UINT> BaseTexture::m_activeTextures;

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

	/* BASE TEXTURE */

	void BaseTexture::updateParameters()
	{
		GLenum Target = (GLenum)m_type;

		glBindTexture(Target, m_id);

		glTexParameteri(Target, GL_TEXTURE_WRAP_S, (GLenum)m_wrapMode);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, (GLenum)m_wrapMode);

		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, (GLenum)m_minFilter);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, (GLenum)m_magFilter);

		if (m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));
	}
	void BaseTexture::updateStorage()
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, (GLenum)m_formatType, m_width, m_height);

		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	void BaseTexture::updateStorage(const GLvoid* pixels)
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, (GLenum)m_formatType, m_width, m_height);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, (GLenum)m_channelType, (GLenum)m_pixelType, pixels);

		//glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_dataType, pixels);
		
		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	void BaseTexture::updateTexImageCubemap(int side, const GLvoid* pixels)
	{
		assert(side < 6);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
			0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_pixelType, pixels
		);

		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	BaseTexture::BaseTexture(
		Texture_Type Type,
		Wrap_Mode WrapMode,
		Min_Filter MinFilter,
		Mag_Filter MagFilter,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Pixel_Type PixelType
	)
		: m_type(Type),
		m_wrapMode(WrapMode),
		m_minFilter(MinFilter),
		m_magFilter(MagFilter),
		m_formatType(FormatType),
		m_channelType(ChannelType),
		m_pixelType(PixelType)
	{
		glGenTextures(1, &m_id);
		assert(m_id != -1);

		m_channelCount = (glUtil::getChannelCount(FormatType, ChannelType));

		updateParameters();
	}
	BaseTexture::~BaseTexture()
	{
		glDeleteTextures(1, &m_id);
	}

	void BaseTexture::Bind(Active_Texture layer) const
	{
		// Update active texture slot
		glUtil::setActiveTexture(layer);

		// Don't bind texture if it hasn't changed
		if (m_activeTextures[m_type] != m_id)
			glBindTexture((GLenum)m_type, m_id);

		// update bound texture
		m_activeTextures[m_type] = m_id;
	}
	void BaseTexture::Bind() const
	{
		// Don't bind texture if it hasn't changed
		if (m_activeTextures[m_type] != m_id)
			glBindTexture((GLenum)m_type, m_id);

		// update bound texture
		m_activeTextures[m_type] = m_id;
	}
	void BaseTexture::Unbind() const
	{
		// Don't bind texture if it hasn't changed
		if (m_activeTextures[m_type] != 0)
			glBindTexture((GLenum)m_type, 0);

		// update bound texture
		m_activeTextures[m_type] = 0;
	}

	void BaseTexture::setWrapMode(Wrap_Mode W)
	{
		m_wrapMode = W;

		updateParameters();
	}
	void BaseTexture::setFilterMode(Min_Filter Min, Mag_Filter Mag)
	{
		m_minFilter = Min;
		m_magFilter = Mag;

		updateParameters();
	}
	void BaseTexture::setBorderColor(Color4F color)
	{
		m_borderColor = color;

		updateParameters();
	}
}