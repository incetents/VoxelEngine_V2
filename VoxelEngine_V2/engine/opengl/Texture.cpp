// Copyright (c) 2018 Emmanuel Lajeunesse
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

	BaseTexture::BaseTexture(
		Texture_Type Type,
		Wrap_Mode WrapMode,
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter
	) : m_type(Type), m_minFilter(MinFilter), m_magFilter(MagFilter)
	{
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
	void BaseTexture::setFilterMode(Filter_Mode_Min Min, Filter_Mode_Mag Mag)
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

	RenderTexture::RenderTexture(int Width, int Height, Format_Type Format, Wrap_Mode WrapMode, Filter_Mode_Min MinFilter, Filter_Mode_Mag MagFilter)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter), m_format(Format)
	{
		m_width = Width;
		m_height = Height;

		glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)Format, m_width, m_height, 0, (GLenum)Format, GL_UNSIGNED_BYTE, NULL);

		Unbind();
	}

	/* TEXTURE */

	Texture::Texture(const std::string& filePath, Wrap_Mode WrapMode, Filter_Mode_Min MinFilter, Filter_Mode_Mag MagFilter, bool InvertY)
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter)
	{
		m_image = SOIL_load_image(filePath.c_str(), &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);

		// Store texture as pixel values
		//std::vector<UCHAR> STORAGE(m_image, m_image + m_width * m_height * m_channels);

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

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath);
		glUtil::setOpenGLName(GL_TEXTURE, m_id, Name);
	}
	Texture::Texture(
		const std::string& name,
		std::vector<Color3F> pixels, UINT width,
		Wrap_Mode WrapMode,
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter
	) 
		: BaseTexture(Texture_Type::TEX_2D, WrapMode, MinFilter, MagFilter)
	{
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

		m_loaded = true;

		GLenum Format = (GLenum)glUtil::getFormatFloat(m_channels);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // helps prevent textures smaller than 4x4 not get corrupted
		glTexImage2D(GL_TEXTURE_2D, 0, Format, m_width, m_height, 0, Format, GL_UNSIGNED_BYTE, m_image);
		glGenerateMipmap(GL_TEXTURE_2D);

		Unbind();

		// glName
		glUtil::setOpenGLName(GL_TEXTURE, m_id, name);
	}
	Texture* Texture::Create(
		const std::string& name,
		const std::string& filePath,
		Wrap_Mode WrapMode,
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter,
		bool InvertY
	)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Texture: " + name);
			return nullptr;
		}

		Texture* T = new Texture(filePath, WrapMode, MinFilter, MagFilter, InvertY);

		if (T->IsLoaded())
			Logger.log("Loaded Texture: " + name);
		else
			Logger.error("Could not Load Texture: " + name);

		m_database.Set(name, T);
		return T;
	}
	Texture* Texture::Create(
		const std::string& name,
		std::vector<Color3F> pixels, UINT width,
		Wrap_Mode WrapMode,
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter
	)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Texture: " + name);
			return nullptr;
		}

		Texture* T = new Texture(name, pixels, width, WrapMode, MinFilter, MagFilter);

		if (T->IsLoaded())
			Logger.log("Loaded Texture: " + name);
		else
			Logger.error("Could not Load Texture: " + name);

		m_database.Set(name, T);
		return T;
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
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter,
		bool InvertY
	) : BaseTexture(Texture_Type::TEX_CUBEMAP, WrapMode, MinFilter, MagFilter)
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

		// glName
		auto Name = stringUtil::nameFromFilepath(filePath1);
		glUtil::setOpenGLName(GL_TEXTURE, m_id, Name);
	}
	Cubemap* Cubemap::Create(
		const std::string& name,
		const std::string& filePath1,
		const std::string& filePath2,
		const std::string& filePath3,
		const std::string& filePath4,
		const std::string& filePath5,
		const std::string& filePath6,
		Wrap_Mode WrapMode,
		Filter_Mode_Min MinFilter,
		Filter_Mode_Mag MagFilter,
		bool InvertY
	)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Cubemap: " + name);
			return nullptr;
		}

		Cubemap* C = new Cubemap(
			filePath1, filePath2, filePath3, filePath4, filePath5, filePath6,
			WrapMode, MinFilter, MagFilter, InvertY
		);

		if (C->IsLoaded())
			Logger.log("Loaded Cubemap: " + name);
		else
			Logger.error("Could not Load Cubemap: " + name);

		m_database.Set(name, C);
		return C;
	}
	Cubemap::~Cubemap()
	{
		for (GLuint i = 0; i < 6; i++)
			delete[] m_image[i];
	}

}