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
#include "../utilities/stringUtil.h"

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
		float m_yOffset;
		float m_advanceFlt;
		float m_bboxWidth;
		float m_bboxHeight;
	};

	class Font
	{
	public:
		FT_Face m_face;
		std::map<char, FontCharacter> m_characters;
		uint32_t m_fontLevel;
		uint32_t m_fontHeight;
		float m_largestYOffset = 0;

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
		// Current Font
		Font* m_font = nullptr;

		// Graphics Resources [ Shared ]
		static FramebufferObject*	m_FBO;
		static VAO*					m_VAO;
		static MeshBufferMem<float>*m_buffer;
		
		// Texture for text
		RenderTexture*	m_renderTexture = nullptr;
		bool			m_renderTextureDirty = true;
		Vector2ui		m_renderTextureTargetSize;
		uint32_t		m_lineCount;
		void			UpdateRenderTextureTargetSize();

	public:
		// Settings
		std::string m_text;
		float		m_scale = 1.0f;

		// Setup
		static void Init();
		static void Destroy();
		static void LoadFont(const std::string& fontName, const std::string& fontPath)
		{
			m_fonts[fontName] = new Font(fontPath, 48);
		}
		
		// Creation
		Text(const std::string& font, const std::string& text)
		{
			if (SetFont(font))
				SetText(text);
			else
				VXL_ASSERT(false, "Text couldn't find font: " + font);
		}
		~Text();

		// Utility
		void			UpdateRenderTexture();
		RenderTexture*	GetRenderTexture(void) const
		{
			// FBO
			if (m_FBO == nullptr)
			{
				m_FBO = FramebufferObject::Create("FUNKY");
				m_FBO->SetSize(m_renderTextureTargetSize.x, m_renderTextureTargetSize.y);
				m_FBO->Bind();
				m_FBO->SetAttachment(0, m_renderTexture);
				m_FBO->checkFBOStatus();
				m_FBO->Unbind();

			}

			return m_renderTexture;
		}
		bool			HasRenderTextureChanged(void) const
		{
			return m_renderTextureDirty;
		}

		// Setters
		inline bool SetFont(const std::string& fontName)
		{
			if (m_fonts.find(fontName) == m_fonts.end())
				return false;

			m_font = m_fonts[fontName];
			return true;
		}
		void		SetText(const std::string& text);

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