// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "TextObject.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../rendering/Geometry.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/VBO.h"
#include "../rendering/MeshBuffer.h"

#include "../textures/Texture.h"

#include "../utilities/stringUtil.h"

namespace Vxl
{
	// Statics
	FramebufferObject*					GlobalRenderText::m_FBO;
	VAO*								GlobalRenderText::m_VAO;
	MeshBufferMem<float>*				GlobalRenderText::m_buffer;
	std::map<std::string, RenderFont*>	RenderFont::m_fonts;

	// Global FreeType Library
	FT_Library FT_GLOBAL;

	RenderFont::RenderFont(const std::string& filepath, uint32_t fontSize)
		: m_fontHeight(fontSize)
	{
		// Font Storage [TEMP]
		FT_Face m_face;

		// Load font
		if (FT_New_Face(FT_GLOBAL, filepath.c_str(), 0, &m_face))
			Logger.error("ERROR::FREETYPE: Failed to load font");

		// Set font size
		FT_Set_Pixel_Sizes(m_face, 0, m_fontHeight);  

		// Fix alignment
		Graphics::Texture::SetPixelUnpackAlignment(PixelAlignment::ALIGN_1);

		// Load all base 128 characters
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load glyph for character
			if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
				Logger.error("ERROR::FREETYTPE: Failed to load Glyph: " + c);

			RenderCharacter charInfo;

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
			charInfo.m_yOffset = (float)(charInfo.m_size.y - charInfo.m_bearing.y);
			charInfo.m_advanceFlt = (float)(m_face->glyph->advance.x >> 6);
			charInfo.m_bboxWidth =  (float)((m_face->bbox.xMax - m_face->bbox.xMin) >> 6);
			charInfo.m_bboxHeight =  (float)((m_face->bbox.yMax - m_face->bbox.yMin) >> 6);

			m_largestYOffset = max(m_largestYOffset, charInfo.m_yOffset);

			m_characters[c] = charInfo;
		}

		m_fontHeight = m_face->size->metrics.height >> 6;

		// Clean up data
		FT_Done_Face(m_face);
	}

	void GlobalRenderText::InitGLResources()
	{
		m_FBO = FramebufferObject::Create("RenderText");

		m_VAO = new VAO();
		m_buffer = new MeshBufferMem<float>();

		BufferLayout layout =
		{
			{BufferType::POSITION, AttributeType::VEC2, false},
			{BufferType::UV, AttributeType::VEC2, false}
		};
		m_buffer->setLayout(layout);
		m_buffer->setBindMode(BufferUsage::DYNAMIC_DRAW);

		// Setup Freetype
		if (FT_Init_FreeType(&FT_GLOBAL))
			Logger.error("ERROR::FREETYPE: Could not init FreeType Library");
	}
	void GlobalRenderText::DestoryGLResources()
	{
		// FBO auto cleaned up
		m_FBO = nullptr;

		delete m_VAO;
		m_VAO = nullptr;

		delete m_buffer;
		m_buffer = nullptr;

		// Close Freetype
		FT_Done_FreeType(FT_GLOBAL);
	}

	void RenderText::Update(const std::string& text)
	{
		// If same font and text not changed, nothing new to update
		if (!m_fontDirty && m_text.compare(text) == 0)
			return;

		m_fontDirty = false;
		m_renderTextureDirty = true;
		m_text = text;
		m_lineCount = stringUtil::countChar(text, '\n') + 1;

		// Update Render Texture
		if (m_renderTexture != nullptr)
			delete m_renderTexture;

		float m_scale = 1.0f;

		float LocalWidth = 0;
		float MaxWidth = 0;

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = m_text.begin(); c != m_text.end(); c++)
		{
			RenderCharacter ch = m_font->m_characters[*c];
		
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
		m_renderTextureTargetSize.y = m_font->m_fontHeight * m_scale * m_lineCount;

		//m_renderTexture = new RenderTexture(m_renderTextureTargetSize.x, m_renderTextureTargetSize.y);
	}
	void RenderText::UpdateRenderTexture()
	{
		//	VXL_RETURN_ON_FAIL(m_font, "Font missing for text: " + m_text);
		//	
		//	// FBO
		//	GlobalRenderText.m_FBO->disableAttachment(0);
		//	GlobalRenderText.m_FBO->setSize(m_renderTextureTargetSize.x, m_renderTextureTargetSize.y);
		//	GlobalRenderText.m_FBO->addAttachment("", m_renderTexture);
		//	GlobalRenderText.m_FBO->complete();
		//	
		//	GlobalRenderText.m_FBO->setClearColor(Color4F(0, 0, 0, 0));
		//	
		//	GlobalRenderText.m_FBO->bind();
		//	GlobalRenderText.m_FBO->clearBuffers();
		//	
		//	
		//	// Vertices
		//	std::vector<float>& Vertices	= m_buffer->getVertices();
		//	bool Resize						= Vertices.size() != 24;
		//	
		//	if (Resize)
		//		Vertices = std::vector<float>(24);
		//	
		//	m_buffer->setVerticesDirty(Resize);
		//	
		//	// VAO
		//	if (m_VAO == nullptr)
		//		m_VAO = new VAO;
		//	
		//	m_VAO->bind();
		//	
		//	float lineHeight = m_font->m_fontLevel * m_scale;
		//	
		//	// Positions (y Starts at top, also offsets y of all text so that characters like lowercase 'q' don't get cut off)
		//	Vector2 Cursor(0, m_font->m_largestYOffset * m_scale + lineHeight * (m_lineCount - 1));
		//	
		//	// Iterate through all characters
		//	std::string::const_iterator c;
		//	for (c = m_text.begin(); c != m_text.end(); c++)
		//	{
		//		FontCharacter ch = m_font->m_characters[*c];
		//	
		//	    float xpos = Cursor.x + ch.m_bearing.x * m_scale;
		//	    float ypos = Cursor.y - (ch.m_yOffset) * m_scale;
		//	
		//		// Special characters
		//		switch (*c)
		//		{
		//		case '\n':
		//			Cursor.y -= lineHeight;
		//			Cursor.x = 0;
		//			continue;
		//		}
		//	
		//		// Skip characters that are empty
		//		if (ch.m_texture)
		//		{
		//			float w = ch.m_size.x * m_scale;
		//			float h = ch.m_size.y * m_scale;
		//			// Update VBO for each character
		//			float vertices[24] = {
		//			   xpos,     ypos + h,   0.0, 0.0,
		//			   xpos,     ypos,       0.0, 1.0,
		//			   xpos + w, ypos,       1.0, 1.0,
		//	
		//			   xpos,     ypos + h,   0.0, 0.0,
		//			   xpos + w, ypos,       1.0, 1.0,
		//			   xpos + w, ypos + h,   1.0, 0.0
		//			};
		//	
		//			// VBO
		//			Vertices = std::vector<float>(vertices, vertices + 24);
		//			m_buffer->setVerticesDirty(false);
		//			m_buffer->bind();
		//	
		//			// Texture
		//			ch.m_texture->Bind();
		//	
		//			// Render
		//			Graphics::Draw::Array(DrawType::TRIANGLES, 6);
		//		}
		//	
		//	    // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		//	    Cursor.x += (ch.m_advance >> 6) * m_scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		//	}
		//	
		//	// VAO
		//	m_VAO->unbind();
		//	
		//	// FBO
		//	m_FBO->unbind();
		//	
		//	// Flag
		//	m_renderTextureDirty = false;
	}

	TextObject::TextObject(const std::string& name)
		: GameObject(name)
	{
		GameObject::SetMaterial(Material::GetAsset("gbuffer_passthroughWorld"));
		GameObject::SetMesh(Geometry.GetCube());
	}
	TextObject::~TextObject()
	{

	}

	TextObject* TextObject::Create(const std::string& name)
	{
		// Create Unique Name
		auto UniqueName = GameObject::FixNameDuplicate(name);
		
		// Create
		TextObject* _entity = new TextObject(UniqueName);
		// Store in entity database
		AddNamedAsset(UniqueName, _entity, AssetMessage::CREATED);
		// Return
		return _entity;
	}

	void TextObject::LoadFont(const std::string& font, const std::string& path)
	{
		RenderFont::m_fonts[font] = new RenderFont(path, 48);
	}
	bool TextObject::SetFont(const std::string& font)
	{
		if (RenderFont::m_fonts.find(font) == RenderFont::m_fonts.end())
		{
			VXL_ASSERT(false, "TextObject cannot find font: " + font);
			return false;
		}

		auto newFont = RenderFont::m_fonts[font];

		if (newFont == renderText.m_font)
			return true;

		renderText.m_font = newFont;
		renderText.m_fontDirty = true;

		return true;
	}
	void TextObject::SetText(const std::string& text)
	{
		VXL_ASSERT(renderText.m_font, "TextObject missing font");

		renderText.Update(stringUtil::trim_copy(text));
	}

	void TextObject::Update()
	{
		GameObject::Update();
	}
	void TextObject::Draw()
	{
		GameObject::Draw();
	}
	void TextObject::TransformChanged()
	{
		//GameObject::TransformChanged();
	}
}