// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "BaseTexture.h"

#include "../rendering/glUtil.h"
#include "../utilities/Macros.h"

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

		// Wrapping
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, (GLenum)m_wrapMode);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, (GLenum)m_wrapMode);

		if (m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));

		// Mag Filter
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, (GLenum)m_filterMode);

		// Min Filter
		if (m_mipMapping)
		{
			if(m_filterMode == Filter_Mode::LINEAR)
				glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

			// Unused Options
			// GL_LINEAR_MIPMAP_NEAREST
			// GL_NEAREST_MIPMAP_LINEAR
		}
		else
			glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, (GLenum)m_filterMode);

		// Anistropic Mode cnanot be larger than Maximum given size
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, min(glInfo.MAX_ANISOTROPY, (float)m_anisotropicMode));
	}
	void BaseTexture::updateStorage()
	{
		glTexStorage2D(GL_TEXTURE_2D, m_mipMapping ? 3 : 1, (GLenum)m_formatType, m_width, m_height);
	}
	void BaseTexture::updateStorage(const GLvoid* pixels)
	{
		glTexStorage2D(GL_TEXTURE_2D, m_mipMapping ? 3 : 1, (GLenum)m_formatType, m_width, m_height);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, (GLenum)m_channelType, (GLenum)m_pixelType, pixels);

		// Legacy
		//glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_dataType, pixels);
	}
	void BaseTexture::updateTexImageCubemap(unsigned int side, const GLvoid* pixels)
	{
		VXL_ASSERT(side < 6, "Side index too large for cubemap");
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
			0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_pixelType, pixels
		);
	}
	void BaseTexture::updateMipmapping()
	{
		if (m_mipMapping)
			glGenerateMipmap((GLenum)m_type);
	}

	BaseTexture::BaseTexture(
		Texture_Type Type,
		Wrap_Mode WrapMode,
		Filter_Mode FilterMode,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Pixel_Type PixelType,
		Anisotropic_Mode AnisotropicMode,
		bool MipMapping
	)
		: m_type(Type),
		m_wrapMode(WrapMode),
		m_filterMode(FilterMode),
		m_formatType(FormatType),
		m_channelType(ChannelType),
		m_pixelType(PixelType),
		m_anisotropicMode(AnisotropicMode),
		m_mipMapping(MipMapping)
	{
		glGenTextures(1, &m_id);
		VXL_ASSERT(m_id != -1, "Base Texture index is -1");

		m_channelCount = glUtil::getChannelCount(ChannelType);
		if (m_channelCount == -1)
			m_channelCount = glUtil::getChannelCount(FormatType);

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
	void BaseTexture::setFilterMode(Filter_Mode filter)
	{
		m_filterMode = filter;

		updateParameters();
	}
	void BaseTexture::setAnistropicMode(Anisotropic_Mode Anso)
	{
		m_anisotropicMode = Anso;

		updateParameters();
	}
	void BaseTexture::setBorderColor(Color4F color)
	{
		m_borderColor = color;

		updateParameters();
	}
}