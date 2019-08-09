// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Text.h"

#include "../utilities/logger.h"



namespace Vxl
{
	std::map<std::string, Font*> Text::m_fonts;

	// Freetype global controller
	FT_Library ft;

	Font::Font(const std::string& filepath)
	{
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_1);

		// Load font
		if (FT_New_Face(ft, filepath.c_str(), 0, &m_face))
			Logger.error("ERROR::FREETYPE: Failed to load font");

		// Set font size
		FT_Set_Pixel_Sizes(m_face, 0, 48);  

		// VBO Layout
		BufferLayout layout =
		{
			{BufferType::POSITION, AttributeType::VEC2, false},
			{BufferType::UV, AttributeType::VEC2, false}
		};
		m_VBO.SetLayout(layout);

		// Load all base 128 characters
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load glyph for character
			if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
				Logger.error("ERROR::FREETYTPE: Failed to load Glyph: " + c);

			FontCharacter charInfo;

			// Create Texture for character [Automatically cleaned up on scene destruction]
			if ((uint32_t)m_face->glyph->bitmap.width == 0) // No texture here
				charInfo.m_texture = nullptr;
			else
			{
				charInfo.m_texture = Texture::CreateCustom(
					std::to_string(c),
					(void*)m_face->glyph->bitmap.buffer,
					(uint32_t)m_face->glyph->bitmap.width,
					(uint32_t)m_face->glyph->bitmap.rows,
					false,
					TextureWrapping::CLAMP_STRETCH,
					TextureFilter::LINEAR,
					TextureFormat::R8,
					TextureChannelType::R,
					TexturePixelType::UNSIGNED_BYTE,
					AnisotropicMode::NONE
				);
			}
			charInfo.m_size = Vector2i(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
			charInfo.m_bearing = Vector2i(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
			charInfo.m_advance = m_face->glyph->advance.x;

			m_characters[c] = charInfo;
		}

		FT_Done_Face(m_face);
	}

	void Text::Init()
	{
		// FREETYPE START //
		if (FT_Init_FreeType(&ft))
			Logger.error("ERROR::FREETYPE: Could not init FreeType Library");
	}
	void Text::Destroy()
	{
		// Destroy Fonts
		for (auto& font : m_fonts)
			delete font.second;

		// FreeType done being used //
		FT_Done_FreeType(ft);
	}

	void Text::Render(const std::string& text, float x, float y, float scale)
	{
		// Font
		Font* font = m_fonts["arial"];

		font->m_VAO.bind();

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			FontCharacter ch = font->m_characters[*c];

		    float xpos = x + ch.m_bearing.x * scale;
		    float ypos = y - (ch.m_size.y - ch.m_bearing.y) * scale;

		    float w = ch.m_size.x * scale;
		    float h = ch.m_size.y * scale;
		    // Update VBO for each character
		    float vertices[24] = {
		       xpos,     ypos + h,   0.0, 0.0,            
		       xpos,     ypos,       0.0, 1.0,
		       xpos + w, ypos,       1.0, 1.0,

		       xpos,     ypos + h,   0.0, 0.0,
		       xpos + w, ypos,       1.0, 1.0,
		       xpos + w, ypos + h,   1.0, 0.0           
		    };

			// VBO
			font->m_VBO.Bind();
			font->m_VBO.SetVertices<float>(vertices, sizeof(vertices), BufferUsage::DYNAMIC_DRAW);

			// Texture
			ch.m_texture->Bind();

			// Render
			Graphics::Draw::Array(DrawType::TRIANGLES, 6);

		    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		    x += (ch.m_advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}

		font->m_VAO.unbind();
	}

	Text::Text(const std::string& text)
	{
		SetText(text);
	}

	void Text::UpdateVertices(float scale)
	{
		//	VXL_RETURN_ON_FAIL(m_font, "Font missing for text: " + m_text);
		//	
		//	// Vertices
		//	std::vector<float>& Vertices	= m_buffer.getVerticesEditing();
		//	bool Resize		= Vertices.size() != 24;
		//	
		//	if (Resize)
		//	{
		//		Vertices = std::vector<float>(24);
		//		//Vertices.reserve(24);
		//		//m_buffer.vertices = std::vector<float>(24);
		//		m_buffer.reload(true);
		//	}
		//	
		//	// Positions
		//	float x = 100, y = 200;
		//	
		//	// Iterate through all characters
		//	std::string::const_iterator c;
		//	for (c = m_text.begin(); c != m_text.end(); c++)
		//	{
		//		FontCharacter ch = m_font->m_characters[*c];
		//	
		//	    float xpos = x + ch.m_bearing.x * scale;
		//	    float ypos = y - (ch.m_size.y - ch.m_bearing.y) * scale;
		//	
		//	    float w = ch.m_size.x * scale;
		//	    float h = ch.m_size.y * scale;
		//	    // Update VBO for each character
		//	    float vertices[24] = {
		//	       xpos,     ypos + h,   0.0, 0.0,            
		//	       xpos,     ypos,       0.0, 1.0,
		//	       xpos + w, ypos,       1.0, 1.0,
		//	
		//	       xpos,     ypos + h,   0.0, 0.0,
		//	       xpos + w, ypos,       1.0, 1.0,
		//	       xpos + w, ypos + h,   1.0, 0.0           
		//	    };
		//	
		//		// VBO
		//		//m_VBO.Bind();
		//		//m_VBO.SetVertices<float>(vertices, sizeof(vertices), BufferUsage::DYNAMIC_DRAW);
		//		//m_buffer.update(vertices);
		//		Vertices = std::vector<float>(vertices, vertices + 24);
		//		m_buffer.reload(false);
		//	
		//		// Texture
		//		ch.m_texture->Bind();
		//	
		//		// Render
		//		Graphics::Draw::Array(DrawType::TRIANGLES, 6);
		//	
		//	    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		//	    x += (ch.m_advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		//	}
		//	
		//	// Update Vertices
		//	m_buffer.reload(Resize);
	}
	void Text::Render(float x, float y)
	{

	}
}