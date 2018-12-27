// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"
#include "../utilities/Database.h"
#include "../math/Color.h"
#include <SOIL/SOIL.h>

#include <string>

namespace Vxl
{
	void FlipTextureY(UCHAR* array, GLuint width, GLuint height, GLuint channels);

	class BaseTexture
	{
	protected:
		GLuint		 m_id = -1;
		int			 m_width;
		int			 m_height;
		int			 m_channels;
		Texture_Type m_type;
		Wrap_Mode	 m_wrapMode;
		Filter_Mode  m_maxFilter;
		Filter_Mode  m_minFilter;
		Color4F		 m_borderColor = Color4F(0, 0, 0, 1);

		void updateParameters();

	public:
		BaseTexture(const BaseTexture&) = delete;
		BaseTexture(
			Texture_Type Type,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR
		);
		virtual ~BaseTexture();

		virtual void Bind();
		virtual void Unbind();

		void setWrapMode(Wrap_Mode W);
		void setFilterMode(Filter_Mode Min, Filter_Mode Max);
		// only works if min filter is [clamp to border]
		void setBorderColor(Color4F color);

		inline GLuint GetID(void) const
		{
			return m_id;
		}
		inline int GetWidth(void) const
		{
			return m_width;
		}
		inline int GetHeight(void) const
		{
			return m_width;
		}
		inline int GetChannelCount(void) const
		{
			return m_channels;
		}
		inline Wrap_Mode GetWrapMode(void) const
		{
			return m_wrapMode;
		}
		inline Filter_Mode GetFilterModeMin(void) const
		{
			return m_minFilter;
		}
		inline Filter_Mode GetFilterModeMax(void) const
		{
			return m_maxFilter;
		}
		inline Color4F GetBorderColor(void) const
		{
			return m_borderColor;
		}
	};

	class RenderTexture : public BaseTexture
	{
	protected:
		Format_Type m_format;
	public:
		RenderTexture(const RenderTexture&) = delete;
		RenderTexture(
			int Width, int Height,
			Format_Type Format = Format_Type::RGBA,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR
		);
	};

	class Texture : public BaseTexture
	{
	protected:
		bool		m_loaded = false;
		UCHAR*		m_image = nullptr;
	public:
		Texture(const Texture&) = delete;
		Texture(
			const std::string& filePath,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR,
			bool InvertY = true
		);
		~Texture();
		// Database
		static Database<Texture> m_database;

		inline bool IsLoaded(void) const
		{
			return m_loaded;
		}
	};

	class Cubemap : public BaseTexture
	{
	protected:
		bool		m_loaded = false;
		UCHAR**		m_image = new UCHAR*[6];
	public:
		Cubemap(const Cubemap&) = delete;
		Cubemap(
			const std::string& filePath1,
			const std::string& filePath2,
			const std::string& filePath3,
			const std::string& filePath4,
			const std::string& filePath5,
			const std::string& filePath6,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR,
			bool InvertY = true
		);
		~Cubemap();
		// Database
		static Database<Cubemap> m_database;

		inline bool IsLoaded(void) const
		{
			return m_loaded;
		}
	};
}

