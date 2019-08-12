// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Text.h"

#include "../utilities/logger.h"
#include "../textures/RenderTexture.h"

#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"

namespace Vxl
{
	// Global Text Objects
	FT_Library ft;
	
	Font::Font(const std::string& filepath, uint32_t fontHeight)
		: m_fontHeight(fontHeight)
	{
		// Load font
		if (FT_New_Face(ft, filepath.c_str(), 0, &m_face))
			Logger.error("ERROR::FREETYPE: Failed to load font");

		// Set font size
		FT_Set_Pixel_Sizes(m_face, 0, fontHeight);  

		// Fix alignment
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_1);

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
					"",
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
			charInfo.m_bboxWidth = (m_face->bbox.xMax - m_face->bbox.xMin) >> 6;
			charInfo.m_bboxHeight = (m_face->bbox.yMax - m_face->bbox.yMin) >> 6;

			m_characters[c] = charInfo;
		}

		FT_Done_Face(m_face);
	}

	// Statics
	std::map<std::string, Font*>	Text::m_fonts;
	FramebufferObject*				Text::m_FBO = nullptr;
	VAO*							Text::m_VAO;
	MeshBufferMem<float>*			Text::m_buffer;

	void Text::Init()
	{
		// Buffer
		if (m_buffer == nullptr)
			m_buffer = new MeshBufferMem<float>();

		BufferLayout layout =
		{
			{BufferType::POSITION, AttributeType::VEC2, false},
			{BufferType::UV, AttributeType::VEC2, false}
		};
		m_buffer->setLayout(layout);
		m_buffer->setBindMode(BufferUsage::DYNAMIC_DRAW);

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

		// FBO is cleaned up automatically
		m_FBO = nullptr;
		delete m_VAO;
		m_VAO = nullptr;
		delete m_buffer;
		m_buffer = nullptr;
	}


	Text::Text(const std::string& text)
	{
		m_text = text;
	}
	Text::~Text()
	{
		delete m_renderTexture;
	}

	Vector2 Text::CalculateTextureSize()
	{
		Vector2 Scale;

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			FontCharacter ch = m_font->m_characters[*c];
		
			Scale.x += ch.m_bboxWidth;
			Scale.y = max(Scale.y, ch.m_bboxHeight);

		    //	float xpos = Cursor.x + ch.m_bearing.x * m_scale;
		    //	float ypos = Cursor.y - (ch.m_size.y - ch.m_bearing.y) * m_scale;
			//	float advance = (ch.m_advance >> 6) * m_scale;
			//	
			//	// Udpate edge values
			//	SizeMin.Min(Vector2(xpos, ypos));
			//	SizeMax.Max(Vector2(xpos + advance, ypos + m_font->m_fontHeight));
			//	
			//	// Special characters
			//	switch (*c)
			//	{
			//	case '\n':
			//		Cursor.y += m_font->m_fontHeight;
			//		Cursor.x = 0;
			//		continue;
			//	}
			//	
			//	// Next glyph
			//	Cursor.x += advance;
		}
		
		//return Vector2(SizeMax - SizeMin);

		return Vector2(1920, 600);

		//return Scale;
	}

	void Text::RenderToFBO()
	{
		VXL_RETURN_ON_FAIL(m_font, "Font missing for text: " + m_text);
		
		// Calculate RenderTexture Size
		Vector2 RenderTextureSize = CalculateTextureSize();

	

		// Texture
		if (m_renderTexture == nullptr)
		{
			m_renderTexture = new RenderTexture(RenderTextureSize.x, RenderTextureSize.y);
		}

		auto id = m_renderTexture->GetID();

		// FBO
		if (m_FBO == nullptr)
		{
			m_FBO = FramebufferObject::Create("asd");
			m_FBO->setSize(RenderTextureSize.x, RenderTextureSize.y);
			m_FBO->addAttachment("", m_renderTexture);
			m_FBO->complete();

			m_FBO->setClearColor(Color4F(0, 0, 0, 1));
		}

		m_FBO->bind();
		m_FBO->clearBuffers();
		

		// Vertices
		std::vector<float>& Vertices	= m_buffer->getVertices();
		bool Resize						= Vertices.size() != 24;
		
		if (Resize)
			Vertices = std::vector<float>(24);

		m_buffer->setVerticesDirty(Resize);
		
		// VAO
		if (m_VAO == nullptr)
			m_VAO = new VAO;

		m_VAO->bind();

		// Positions
		Vector2 Cursor(0, m_font->m_fontHeight); // { 0, 0 }
		
		// Iterate through all characters
		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			FontCharacter ch = m_font->m_characters[*c];
		
		    float xpos = Cursor.x + ch.m_bearing.x * m_scale;
		    float ypos = Cursor.y - (ch.m_size.y - ch.m_bearing.y) * m_scale;
		
			// Special characters
			switch (*c)
			{
			case '\n':
				Cursor.y -= m_font->m_fontHeight;
				Cursor.x = 0;
				continue;
			}

			// Skip characters that are empty
			if (ch.m_texture)
			{
				float w = ch.m_size.x * m_scale;
				float h = ch.m_size.y * m_scale;
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
				Vertices = std::vector<float>(vertices, vertices + 24);
				m_buffer->setVerticesDirty(false);
				m_buffer->bind();

				// Texture
				ch.m_texture->Bind();

				// Render
				Graphics::Draw::Array(DrawType::TRIANGLES, 6);
			}
		
		    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		    Cursor.x += (ch.m_advance >> 6) * m_scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		
		// VAO
		m_VAO->unbind();

		// FBO
		m_FBO->unbind();

		// Update Vertices
		//m_buffer.set(Resize);
	}
	//void Text::Render(float x, float y)
	//{
	//
	//}
}