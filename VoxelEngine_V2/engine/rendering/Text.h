// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../textures/Texture.h"
#include "../math/Vector.h"
#include "../rendering/VBO.h"
#include "../rendering/MeshBuffer.h"
#include "../modules/Component.h"

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
	protected:
		static std::map<std::string, Font*> m_fonts;

		VBO			m_VBO;
		Font*		m_font = nullptr;
		std::string m_text;
		MeshBufferMem<float> m_buffer;
	public:
		// DEBUG
		static void Init();
		static void Destroy();
		static void Render(const std::string& text, float x, float y, float scale);
		
		// Setup
		static void LoadFont(const std::string& fontName, const std::string& fontPath)
		{
			m_fonts[fontName] = new Font(fontPath);
		}
		
		// Creation
		Text(const std::string& text);

		// Utility
		void UpdateVertices();
		void Render() {}

		// Setters
		inline void SetFont(const std::string& fontName)
		{
			m_font = m_fonts[fontName];
		}
		inline void SetText(const std::string& text)
		{
			m_text = text;
		}

		// Getters
		inline Font* GetFont(void) const
		{
			return m_font;
		}
		inline std::string GetText(void) const
		{
			return m_text;
		}
	};
}