// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"

#include "../textures/Texture.h"
#include "../textures/RenderTexture.h"
#include "../utilities/logger.h"
#include "RenderBuffer.h"
#include "../window/window.h"

#include <assert.h>

namespace Vxl
{
	GLuint FramebufferObject::m_boundID = 0;

	void FramebufferObject::FBOTexture::load(int Width, int Height)
	{
		unload();

		if (m_type == FBORenderType::TEXTURE)
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

	void FramebufferObject::FBOTexture::unload()
	{
		if (!m_empty)
		{
			if (m_type == FBORenderType::BUFFER)
				delete m_buffer;
			else if (m_type == FBORenderType::TEXTURE)
				delete m_texture;

			m_empty = true;
		}
	}

	GLuint FramebufferObject::FBOTexture::GetID(void) const
	{
		if (m_type == FBORenderType::BUFFER)
			return m_buffer->GetID();

		return m_texture->GetID();
	}
	Format_Type	FramebufferObject::FBOTexture::GetFormatType(void) const
	{
		if (m_type == FBORenderType::BUFFER)
			return m_buffer->GetFormatType();

		return m_texture->GetFormatType();
	}
	Channel_Type FramebufferObject::FBOTexture::GetChannelType(void) const
	{
		if (m_type == FBORenderType::BUFFER)
			return m_buffer->GetChannelType();

		return m_texture->GetChannelType();
	}
	Pixel_Type FramebufferObject::FBOTexture::GetPixelType(void) const
	{
		if (m_type == FBORenderType::BUFFER)
			return m_buffer->GetPixelType();

		return m_texture->GetPixelType();
	}
	int FramebufferObject::FBOTexture::GetChannelCount(void) const
	{
		if (m_type == FBORenderType::BUFFER)
			return m_buffer->GetChannelCount();

		return m_texture->GetChannelCount();
	}

	void FramebufferObject::FBOTexture::Bind()
	{
		if (m_type == FBORenderType::BUFFER)
			m_buffer->Bind();
		else
			m_texture->Bind();
	}
	void FramebufferObject::FBOTexture::Unbind()
	{
		if (m_type == FBORenderType::BUFFER)
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
			assert(false);

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
		assert(index < m_textureCount);
		glClearBufferfv(GL_COLOR, index, &m_clearColor[0]);
	}

	void FramebufferObject::addTexture(
		const std::string& name,
		Format_Type FormatType,
		FBORenderType fboRenderType
	)
	{
		assert(m_textureCount < (GLuint)glUtil::GetMaxFBOColorAttachments());
		
		// Create new render texture
		m_textures.push_back(new FBOTexture(name, fboRenderType, FormatType, Channel_Type::RGBA, Pixel_Type::UNSIGNED_BYTE));
		m_textureCount++;

		//	if(fboRenderType == FBORenderType::TEXTURE)
//		m_textures[m_textureCount].BecomeRenderTexture(m_size[0], m_size[1], FormatType);
//	else
//		m_textures[m_textureCount].BecomeRenderBuffer(m_size[0], m_size[1], FormatType);
//	
//	// Name
//	m_textures[m_textureCount].m_name = name;
//	glUtil::setGLName(glNameType::TEXTURE, m_textures[m_textureCount].GetID(), "FBO_" + m_name + "_Tex_" + name);
//	
//	// Add to FBO
//	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
//	
//	if (fboRenderType == FBORenderType::TEXTURE)
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_textureCount++), GL_TEXTURE_2D, m_textures[m_textureCount].GetID(), 0);
//	else
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_textureCount++), GL_RENDERBUFFER, m_textures[m_textureCount].GetID());
//	
//	checkFBOStatus();
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FramebufferObject::addDepth(DepthFormat_Type depthFormatType, FBORenderType fboRenderType)
	{
		assert(m_depth == nullptr);

		// Select Pixel/Channel Type [for glReadPixels]
		Pixel_Type PixelType;
		Channel_Type ChannelType;
		glUtil::getPixelChannelData(depthFormatType, ChannelType, PixelType);

		m_depth = new FBOTexture("depth", fboRenderType, (Format_Type)depthFormatType, ChannelType, PixelType);

		//	if (fboRenderType == FBORenderType::TEXTURE)
		//		m_depth.BecomeRenderTexture(m_size[0], m_size[1], (Format_Type)depthFormatType, ChannelType, PixelType);
		//	else
		//		m_depth.BecomeRenderBuffer(m_size[0], m_size[1], (Format_Type)depthFormatType, ChannelType, PixelType);
		//	
		//	// Name
		//	glUtil::setGLName(glNameType::TEXTURE, m_depth.GetID(), "FBO_" + m_name + "_Depth");
		//	
		//	// Add to FBO
		//	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		//	
		//	if (fboRenderType == FBORenderType::TEXTURE)
		//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth.GetID(), 0);
		//	else
		//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth.GetID());
		//	
		//	checkFBOStatus();
		//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	void FramebufferObject::bindTexture(int index, Active_Texture layer)
	{
		assert(index < (int)m_textureCount && m_textures[index]->isRenderTexture());

		glUtil::setActiveTexture(layer);

		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		assert(m_depth->isRenderTexture());

		glUtil::setActiveTexture(layer);
		m_depth->Bind();
	}

	void FramebufferObject::copyDepth(const FramebufferObject& fbo)
	{
		// Must have matching formats
		assert(m_depth->GetFormatType() == fbo.m_depth->GetFormatType());

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.m_id);

		glBlitFramebuffer(
			0, 0, m_width, m_height,
			0, 0, fbo.m_width, fbo.m_height,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	RawArray<GLubyte> FramebufferObject::readPixels(u_int textureIndex, int x, int y, int w, int h)
	{
		assert(textureIndex < m_textureCount);
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
	RawArray<GLubyte> FramebufferObject::readDepthPixel(int x, int y, int w, int h)
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
}
