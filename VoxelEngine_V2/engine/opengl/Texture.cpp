// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Texture.h"

#include <assert.h>

#include "glUtil.h"
#include "../utilities/logger.h"
#include "../utilities/stringUtil.h"
#include "../math/Color.h"

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
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, (GLenum)m_magFilter);

		if (m_wrapMode == Wrap_Mode::CLAMP_BORDER)
			glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, &(m_borderColor[0]));
	}
	void BaseTexture::updateTexImage2D(const GLvoid* pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_dataType, pixels);
	
		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	void BaseTexture::updateTexImageCubemap(int side, const GLvoid* pixels)
	{
		assert(side < 6);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
			0, (GLenum)m_formatType, m_width, m_height, 0, (GLenum)m_channelType, (GLenum)m_dataType, pixels
		);

		if (m_useMipMapping)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	BaseTexture::BaseTexture(
		Texture_Type Type,
		Wrap_Mode WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Data_Type DataType
	) 
		: m_type(Type),
		m_wrapMode(WrapMode),
		m_minFilter(MinFilter),
		m_magFilter(MagFilter),
		m_formatType(FormatType),
		m_channelType(ChannelType),
		m_dataType(DataType)
	{
		glGenTextures(1, &m_id);

		updateParameters();
	}
	BaseTexture::~BaseTexture()
	{
		if(m_id != -1)
			glDeleteTextures(1, &m_id);
	}

	void BaseTexture::Bind(Active_Texture layer) const
	{
		glActiveTexture((GLenum)layer);
		glBindTexture((GLenum)m_type, m_id);
	}
	void BaseTexture::Bind() const
	{
		glBindTexture((GLenum)m_type, m_id);
	}
	void BaseTexture::Unbind() const
	{
		glBindTexture((GLenum)m_type, 0);
	}

	void BaseTexture::setWrapMode(Wrap_Mode W)
	{
		m_wrapMode = W;

		updateParameters();
	}
	void BaseTexture::setFilterMode(Filter_Min Min, Filter_Mag Mag)
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

	/* RENDER TEXTURE */

	RenderTexture::RenderTexture(
		int Width, int Height,
		Wrap_Mode WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Data_Type DataType
	)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, DataType)
	{
		m_width = Width;
		m_height = Height;

		updateTexImage2D();
		Unbind();
	}

	/* TEXTURE */

	Texture::Texture(
		const std::string& filePath,
		bool			InvertY,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type		FormatType,
		Data_Type		DataType
	)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter, FormatType, Channel_Type::NONE, DataType)
	{
		m_useMipMapping = UseMipMapping;

		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		if (!m_image)
		{
			Logger.error("Could not load Image: " + filePath);
			Logger.error("SOIL: " + std::string(SOIL_last_result()));
			return;
		}

		// Get Correct channel data
		m_channelType = glUtil::getFormat(m_channels);

		// Invert Y
		if (InvertY)
			FlipTextureY(m_image, m_width, m_height, m_channels);

		// Storage
		updateTexImage2D(&m_image[0]);

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath);
		glUtil::setOpenGLName(glNameType::TEXTURE, m_id, "Tex_" + Name);

		// finished
		Unbind();
		m_loaded = true;
	}
	Texture::Texture(
		const std::string& name,
		std::vector<Color3F> pixels, UINT width,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type		FormatType,
		Data_Type		DataType
	) 
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter, FormatType, Channel_Type::RGB, DataType)
	{
		m_useMipMapping = UseMipMapping;

		UINT pixelsCount = (UINT)pixels.size();
		m_width = width;
		m_height = pixelsCount / width;
		m_channels = 3;

		std::vector<UCHAR> fixed_pixels;
		fixed_pixels.reserve(pixelsCount * m_channels);

		for (UINT i = 0; i < pixelsCount; i++)
		{
			fixed_pixels.push_back((UCHAR)(pixels[i].r * 255.0f));
			fixed_pixels.push_back((UCHAR)(pixels[i].g * 255.0f));
			fixed_pixels.push_back((UCHAR)(pixels[i].b * 255.0f));
		}

		m_image = new UCHAR[pixelsCount * m_channels];
		memcpy(m_image, &fixed_pixels[0], fixed_pixels.size() * sizeof(UCHAR));

		// Storage
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // helps prevent textures smaller than 4x4 not get corrupted
		updateTexImage2D(&m_image[0]);

		// glName
		glUtil::setOpenGLName(glNameType::TEXTURE, m_id, "Tex_" + name);

		// finished
		Unbind();
		m_loaded = true;
	}

	Texture* Texture::CreateCustom(
		const std::string& name,
		std::vector<Color3F> pixels, UINT width,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type		FormatType,
		Data_Type		DataType
	) {
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Texture: " + name);
			return nullptr;
		}

		Texture* Tex = new Texture(name, pixels, width, UseMipMapping, WrapMode, MinFilter, MagFilter, FormatType, DataType);

		if (Tex == nullptr)
			return false;
		else if (!Tex->IsLoaded())
			return false;

		Logger.log("Loaded Texture: " + name);
		Texture::m_database.Set(name, Tex);

		return Tex;
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
		bool			InvertY,
		bool			UseMipMapping,
		Wrap_Mode		WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type		FormatType,
		Data_Type		DataType
	) 
		: BaseTexture(Texture_Type::TEX_CUBEMAP, WrapMode, MinFilter, MagFilter, FormatType, Channel_Type::NONE, DataType)
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

		//if(m_useMipMapping)
		//	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath1);
		glUtil::setOpenGLName(glNameType::TEXTURE, m_id, "Cubemap_" + Name);

		// finished
		Unbind();
		m_loaded = true;
	}
	Cubemap::~Cubemap()
	{
		for (GLuint i = 0; i < 6; i++)
			delete[] m_image[i];
	}

}