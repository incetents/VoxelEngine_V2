// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../textures/Texture.h"
#include "../math/Vector.h"
#include "../rendering/VBO.h"

#include <map>
#include <string>

namespace Vxl
{
	struct FontCharacter
	{
		Texture* m_texture;
		Vector2i m_size;
		Vector2i m_bearing;
		uint32_t m_advance;
	};

	class Font
	{
	public:
		FT_Face m_face;
		std::map<char, FontCharacter> m_characters;

		VAO m_VAO;
		VBO	m_VBO;
	public:
		Font(const std::string& filepath);

		inline FT_Face GetFace(void) const
		{
			return m_face;
		}
	};

	class Text
	{
	public:
		static std::map<std::string, Font*> m_fonts;
		static void Init();

		static void Render(const std::string& text, float x, float y, float scale);
	};
}