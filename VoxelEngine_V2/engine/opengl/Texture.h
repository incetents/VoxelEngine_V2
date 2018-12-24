// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Enums.h"
#include "../utilities/Database.h"
#include "../math/Color.h"
#include <SOIL/SOIL.h>

#include <string>

namespace Vxl
{
	class BaseTexture
	{
	protected:
		GLuint		m_id = -1;
		int			m_width;
		int			m_height;
		int			m_channels;
		Wrap_Mode	m_wrapMode;
		Filter_Mode m_maxFilter;
		Filter_Mode m_minFilter;
		Color4F		m_borderColor = Color4F(0, 0, 0, 1);

		void updateParameters();

	public:
		BaseTexture(
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR
		);
		virtual ~BaseTexture();

		BaseTexture(const BaseTexture&) = delete;

		void Bind();
		static void Unbind();

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
		RenderTexture(
			int Width, int Height,
			Format_Type Format = Format_Type::RGBA,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR
		);

		RenderTexture(const RenderTexture&) = delete;
	};

	class Texture : public BaseTexture
	{
	protected:
		bool		m_loaded = false;
		UCHAR*		m_image = nullptr;
	public:
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

		Texture(const Texture&) = delete;

		inline bool IsLoaded(void) const
		{
			return m_loaded;
		}
	};
}

