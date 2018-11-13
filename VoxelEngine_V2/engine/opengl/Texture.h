// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once
#include "Enums.h"

#include "../utilities/Database.h"
#include "../math/Color.h"
#include <SOIL/SOIL.h>

#include <string>

namespace Vxl
{
	class Texture
	{
	private:
		GLuint		m_id;
		int			m_width;
		int			m_height;
		int			m_channels;
		UCHAR*		m_image = nullptr;
		Wrap_Mode	m_wrapMode;
		Filter_Mode m_maxFilter;
		Filter_Mode m_minFilter;
		Color4F		m_borderColor = Color4F(0,0,0,1);

		void update();

		// Databse
		Database<Texture> m_database;
	public:
		Texture(
			const std::string& filePath,
			Wrap_Mode WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode MinFilter = Filter_Mode::LINEAR,
			Filter_Mode MaxFilter = Filter_Mode::LINEAR
		);
		~Texture();

		void setWrapMode(Wrap_Mode W);
		void setFilterMode(Filter_Mode Min, Filter_Mode Max);
		void setBorderColor(Color4F color);

		void bind();
		static void unbind();
	};
}

