// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"
#include "RenderBuffer.h"

#include "../textures/Texture.h"
#include "../textures/RenderTexture.h"
#include "../utilities/logger.h"
#include "../window/window.h"
#include "../input/Input.h"

namespace Vxl
{
	GLuint FramebufferObject::m_boundID = 0;

	void FramebufferObject::Attachment::load(int Width, int Height)
	{
		unload();

		if (m_type == Attachment_Type::TEXTURE)
		{
			m_texture = new RenderTexture(Width, Height, m_formatType, m_channelType, m_pixelType);
			glUtil::setGLName(glNameType::TEXTURE, m_texture->GetID(), "FBO_" + m_name + "_Tex_" + m_name);
		}
		else
		{
			m_buffer = new RenderBuffer(Width, Height, m_formatType, m_channelType, m_pixelType);
			glUtil::setGLName(glNameType::TEXTURE, m_buffer->GetID(), "FBO_" + m_name + "_RenderBuf_" + m_name);
		}


		m_empty = false;
	}

	void FramebufferObject::Attachment::unload()
	{
		if (!m_empty)
		{
			if (m_type == Attachment_Type::BUFFER)
				delete m_buffer;
			else if (m_type == Attachment_Type::TEXTURE)
				delete m_texture;

			m_empty = true;
		}
	}

	GLuint FramebufferObject::Attachment::GetID(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_buffer->GetID();

		return m_texture->GetID();
	}
	Format_Type	FramebufferObject::Attachment::GetFormatType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_buffer->GetFormatType();

		return m_texture->GetFormatType();
	}
	Channel_Type FramebufferObject::Attachment::GetChannelType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_buffer->GetChannelType();

		return m_texture->GetChannelType();
	}
	Pixel_Type FramebufferObject::Attachment::GetPixelType(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_buffer->GetPixelType();

		return m_texture->GetPixelType();
	}
	int FramebufferObject::Attachment::GetChannelCount(void) const
	{
		if (m_type == Attachment_Type::BUFFER)
			return m_buffer->GetChannelCount();

		return m_texture->GetChannelCount();
	}

	void FramebufferObject::Attachment::Bind()
	{
		if (m_type == Attachment_Type::BUFFER)
			m_buffer->Bind();
		else
			m_texture->Bind();
	}
	void FramebufferObject::Attachment::Unbind()
	{
		if (m_type == Attachment_Type::BUFFER)
			m_buffer->Unbind();
		else
			m_texture->Unbind();
	}

	FramebufferObject::FramebufferObject(const std::string& name)
		: m_name(name)
	{
		
	}

	FramebufferObject* FramebufferObject::Create(
		const std::string& name
	){
		FramebufferObject* _FBO = new FramebufferObject(name);

		AddToDatabase(name, _FBO);
		Message_Created(name, _FBO);

		return _FBO;
	}

	FramebufferObject::~FramebufferObject()
	{
		unload();
	}

	void FramebufferObject::load()
	{
		// Already init
		if (m_id != -1)
		{
			VXL_ASSERT(false, "FBO already init");
			return;
		}

		// Create FBO
		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glUtil::setGLName(glNameType::FRAMEBUFFER, m_id, "FBO_" + m_name);

		// Color attachments
		for (unsigned int i = 0; i < m_textureCount; i++)
		{
			m_textures[i]->load(m_width, m_height);

			if (m_textures[i]->isRenderTexture())
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->GetID(), 0);
			else
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, m_textures[i]->GetID());
		}

		// Depth attachment
		m_depth->load(m_width, m_height);
		if (m_depth->isRenderTexture())
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->GetID(), 0);
		else
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth->GetID());

		// Final Check
		checkFBOStatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FramebufferObject::unload()
	{
		if (m_id != -1)
		{
			glDeleteFramebuffers(1, &m_id);
			m_id = -1;

			// Destroy attached textures and depth/stencil
			for (auto& tex : m_textures)
				tex->unload();
			m_depth->unload();
		}
	}

	bool FramebufferObject::checkFBOStatus()
	{
		// Check if FBO was Created Correctly
		GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		switch (e) {

		case GL_FRAMEBUFFER_UNDEFINED:
			Logger.error("FBO Undefined\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			Logger.error("FBO Incomplete Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			Logger.error("FBO Missing Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			Logger.error("FBO Incomplete Draw Buffer\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			Logger.error("FBO Unsupported\n");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			//FBO is OK
			break;

		default:
			//Unknown FBO
			Logger.error("FBO Error, Unknown: " + std::to_string(e));
			break;
		}

		// Return true if framebuffer complete
		return (e == GL_FRAMEBUFFER_COMPLETE) ? true : false;
	}

	void FramebufferObject::bindFBO()
	{
		// Don't bind FBO if it hasn't changed
		if (m_boundID != m_id)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			glDrawBuffers(m_textureCount, _Global_Color_Attachment_Calls);
		}
		// update bound FBO
		m_boundID = m_id;
	}
	void FramebufferObject::setSizeToWindowSize()
	{
		m_width = Window.GetResolutionWidth();
		m_height = Window.GetResolutionHeight();
		m_fullscreen = true;
	}
	void FramebufferObject::clearBuffers()
	{
		glUtil::clearColor(m_clearColor);
		glUtil::clearBuffer(m_clearBuffers);
	}
	void FramebufferObject::clearBuffer(unsigned int index)
	{
		VXL_ASSERT(index < m_textureCount, "FBO, Index out of bounds");
		glClearBufferfv(GL_COLOR, index, &m_clearColor[0]);
	}

	void FramebufferObject::addTexture(
		const std::string& name,
		Format_Type FormatType,
		Attachment_Type fboRenderType
	)
	{
		VXL_ASSERT(m_textureCount < (GLuint)glUtil::GetMaxFBOColorAttachments(), "FBO, too many color attachments");
		
		// Create new render texture
		m_textures.push_back(new Attachment(name, fboRenderType, FormatType, Channel_Type::RGBA, Pixel_Type::UNSIGNED_BYTE));
		m_textureCount++;
	}
	void FramebufferObject::addDepth(DepthFormat_Type depthFormatType, Attachment_Type fboRenderType)
	{
		VXL_ASSERT(m_depth == nullptr, "FBO, cannot add multiple depth/stencil attachments");

		// Select Pixel/Channel Type [for glReadPixels]
		Pixel_Type PixelType;
		Channel_Type ChannelType;
		glUtil::getPixelChannelData(depthFormatType, ChannelType, PixelType);

		m_depth = new Attachment("depth", fboRenderType, (Format_Type)depthFormatType, ChannelType, PixelType);
	}

	void FramebufferObject::bind()
	{
		bindFBO();

		glUtil::viewport(0, 0, m_width, m_height);
	}
	void FramebufferObject::bind(UINT viewportX, UINT viewportY, UINT viewportW, UINT viewportH)
	{
		bindFBO();

		glUtil::viewport(viewportX, viewportY, viewportW, viewportH);
	}
	void FramebufferObject::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_boundID = 0;
	}

	void FramebufferObject::bindTexture(unsigned int index, Active_Texture layer)
	{
		VXL_ASSERT(index < m_textureCount, "FBO, bindTexture index out of bounds");
		VXL_ASSERT(m_textures[index]->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		glUtil::setActiveTexture(layer);

		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		VXL_ASSERT(m_depth->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		glUtil::setActiveTexture(layer);
		m_depth->Bind();
	}

	void FramebufferObject::blitDepth(const FramebufferObject& fbo)
	{
		// Must have matching formats
		VXL_ASSERT(m_depth->GetFormatType() == fbo.m_depth->GetFormatType(), "FBO, blitDepth doesn't have matching depth formats");
		// Must have matching sizes
		VXL_ASSERT(m_width == fbo.m_width && m_height == fbo.m_height, "FBO, blitDepth doesn't have matching sizes");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.m_id);

		glBlitFramebuffer(
			0, 0, m_width, m_height,
			0, 0, fbo.m_width, fbo.m_height,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	// Generates mipmap of current texture
	void FramebufferObject::generateMipmaps(unsigned int textureIndex)
	{
		VXL_ASSERT(textureIndex < m_textureCount, "FBO, index out of bounds");
		VXL_ASSERT(m_id == m_boundID, "FBO" + m_name + "must be bound before generating mipmaps for its color attachments");
		VXL_ASSERT(m_textures[textureIndex]->isRenderTexture(), "FBO, bindTexture is not RenderTexture");

		// Create mipmapping for Fbo Texture
		m_textures[textureIndex]->m_texture->updateMipmapping();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<GLubyte> FramebufferObject::readPixels(u_int textureIndex, int x, int y, int w, int h)
	{
		VXL_ASSERT(textureIndex < m_textureCount, "FBO readpixels: index out of bounds");
		
		// Ignore if x,y coordinates are outside FBO range
		if (x < 0 || y < 0)
			return RawArray<GLubyte>();

		auto Texture = m_textures[textureIndex];
		
		RawArray<GLubyte> Array;
		Array.start = new GLubyte[w * h * Texture->GetChannelCount()];

		glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
		glReadPixels(
			x, y, w, h,
			(GLenum)Texture->GetChannelType(),
			(GLenum)Texture->GetPixelType(),
			Array.start
		);

		return Array;
	}
	RawArray<GLubyte> FramebufferObject::readPixelsFromMouse(u_int textureIndex, int w, int h)
	{
		float px, py;
		px = Input.getMousePosViewportX();  // [0 -> 1] horizontally across viewport
		py = Input.getMousePosViewportY(true);  // [0 -> 1] vertically across viewport

		// Move position from [0->1] into [0->width] and [0->height]
		px *= m_width;
		py *= m_height;

		return readPixels(textureIndex, (int)px, (int)py, w, h);
	}

	RawArray<GLubyte> FramebufferObject::readDepthPixels(int x, int y, int w, int h)
	{
		RawArray<GLubyte> Array;
		Array.start = new GLubyte[w * h * m_depth->GetChannelCount()];

		glReadPixels(
			x, y, w, h,
			(GLenum)m_depth->GetChannelType(),
			(GLenum)m_depth->GetPixelType(),
			Array.start
		);

		return Array;
	}
	RawArray<GLubyte> FramebufferObject::readDepthPixelsFromMouse(int w, int h)
	{
		float px, py;
		px = Input.getMousePosViewportX();  // [0 -> 1] horizontally across viewport
		py = Input.getMousePosViewportY(true);  // [0 -> 1] vertically across viewport

		// Move position from [0->1] into [0->width] and [0->height]
		px *= m_width;
		py *= m_height;

		return readDepthPixels((int)px, (int)py, w, h);
	}
}
