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
	
	Font::Font(const std::string& filepath, uint32_t fontLevel)
		: m_fontLevel(fontLevel)
	{
		// Load font
		if (FT_New_Face(ft, filepath.c_str(), 0, &m_face))
			Logger.error("ERROR::FREETYPE: Failed to load font");

		// Set font size
		FT_Set_Pixel_Sizes(m_face, 0, fontLevel);  

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
			charInfo.m_yOffset = charInfo.m_size.y - charInfo.m_bearing.y;
			charInfo.m_advanceFlt = m_face->glyph->advance.x >> 6;
			charInfo.m_bboxWidth = (m_face->bbox.xMax - m_face->bbox.xMin) >> 6;
			charInfo.m_bboxHeight = (m_face->bbox.yMax - m_face->bbox.yMin) >> 6;

			m_largestYOffset = max(m_largestYOffset, charInfo.m_yOffset);

			m_characters[c] = charInfo;
		}

		m_fontHeight = m_face->size->metrics.height >> 6;

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

	Text::~Text()
	{
		
	}

	void Text::UpdateRenderTextureTargetSize()
	{
		m_renderTextureTargetSize = Vector2ui(0, 0);

		float LocalWidth = 0;
		float MaxWidth = 0;

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			FontCharacter ch = m_font->m_characters[*c];
		
			// Special characters
			switch (*c)
			{
			case '\n':
				LocalWidth = 0;
				continue;
			}

			LocalWidth += ch.m_advanceFlt * m_scale;
			MaxWidth = max(MaxWidth, LocalWidth);
		}
		
		m_renderTextureTargetSize.x = MaxWidth;
		m_renderTextureTargetSize.y = m_font->m_fontLevel * m_scale * m_lineCount;

		// Update Render Texture
		if (m_renderTexture != nullptr)
			RenderTexture::DeleteUnnamedAsset(m_renderTexture);

		m_renderTexture = RenderTexture::Create(m_renderTextureTargetSize.x, m_renderTextureTargetSize.y);
	}

	void Text::UpdateRenderTexture()
	{
		VXL_RETURN_ON_FAIL(m_font, "Font missing for text: " + m_text);
		
		m_FBO->Bind();
		m_FBO->DrawToBuffers();
		m_FBO->ClearBuffers();
		
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

		float lineHeight = m_font->m_fontLevel * m_scale;

		// Positions (y Starts at top, also offsets y of all text so that characters like lowercase 'q' don't get cut off)
		Vector2 Cursor(0, m_font->m_largestYOffset * m_scale + lineHeight * (m_lineCount - 1));
		
		// Iterate through all characters
		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			FontCharacter ch = m_font->m_characters[*c];
		
		    float xpos = Cursor.x + ch.m_bearing.x * m_scale;
		    float ypos = Cursor.y - (ch.m_yOffset) * m_scale;
		
			// Special characters
			switch (*c)
			{
			case '\n':
				Cursor.y -= lineHeight;
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
		m_FBO->Unbind();

		// Flag
		m_renderTextureDirty = false;
	}

	void Text::SetText(const std::string& text)
	{
		std::string newString = text;
		stringUtil::trim(newString);

		// No change
		if (m_text.compare(newString) == 0)
			return;

		m_text = newString;
		m_lineCount = stringUtil::countChar(m_text, '\n') + 1;
		m_renderTextureDirty = true;
		UpdateRenderTextureTargetSize();
	}
}