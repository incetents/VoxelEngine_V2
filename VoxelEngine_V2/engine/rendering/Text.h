// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../rendering/VBO.h"
#include "../rendering/MeshBuffer.h"
#include "../rendering/FramebufferObject.h"
#include "../textures/Texture.h"
#include "../modules/Component.h"
#include "../math/Vector.h"

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
		float m_bboxWidth;
		float m_bboxHeight;
	};

	class Font
	{
	public:
		FT_Face m_face;
		std::map<char, FontCharacter> m_characters;
		uint32_t m_fontHeight;

	public:
		Font(const std::string& filepath, uint32_t fontHeight);

		inline FT_Face GetFace(void) const
		{
			return m_face;
		}
	};

	class Text
	{
	protected:
		// Fonts
		static std::map<std::string, Font*> m_fonts;

		// Graphics Resources [ Shared ]
		static FramebufferObject*	m_FBO;
		static VAO*					m_VAO;
		static MeshBufferMem<float>*m_buffer;
		// Texture for text
		RenderTexture* m_renderTexture = nullptr;

		Font* m_font = nullptr;
		
		// How much space is required for renderTexture
		Vector2 CalculateTextureSize();

	public:
		// Settings
		std::string m_text;
		float m_scale = 10.0f;

		// Setup
		static void Init();
		static void Destroy();
		static void LoadFont(const std::string& fontName, const std::string& fontPath)
		{
			m_fonts[fontName] = new Font(fontPath, 48);
		}
		
		// Creation
		Text(const std::string& text);
		~Text();

		// Utility
		void RenderToFBO();
		//void Render();

		RenderTexture* GetRenderTexture(void) const
		{
			return m_renderTexture;
		}

		// Setters
		inline void SetFont(const std::string& fontName)
		{
			m_font = m_fonts[fontName];
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