// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"

#include "../textures/Texture.h"
#include "../textures/RenderTexture.h"
#include "../utilities/logger.h"
#include "RenderBuffer.h"

#include <assert.h>

namespace Vxl
{
	GLuint FramebufferObject::m_boundID = 0;

	void FramebufferObject::FBOTexture::BecomeRenderTexture(
		int Width, int Height,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Pixel_Type PixelType
	)
	{
		if (m_type == Type::BUFFER)
			delete m_buffer;
		else if (m_type == Type::TEXTURE)
			delete m_texture;

		m_texture = new RenderTexture(Width, Height, FormatType, ChannelType, PixelType);
		m_type = Type::TEXTURE;
	}

	void FramebufferObject::FBOTexture::BecomeRenderBuffer(
		int Width, int Height,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Pixel_Type PixelType
	)
	{
		if (m_type == Type::BUFFER)
			delete m_buffer;
		else if (m_type == Type::TEXTURE)
			delete m_texture;

		m_buffer = new RenderBuffer(Width, Height, FormatType, ChannelType, PixelType);
		m_type = Type::BUFFER;
	}

	GLuint FramebufferObject::FBOTexture::GetID()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			return m_buffer->GetID();

		return m_texture->GetID();
	}
	int FramebufferObject::FBOTexture::GetChannelCount()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			return m_buffer->GetChannelCount();

		return m_texture->GetChannelCount();

	}
	Channel_Type FramebufferObject::FBOTexture::GetChannelType()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			return m_buffer->GetChannelType();

		return m_texture->GetChannelType();
	}
	Pixel_Type FramebufferObject::FBOTexture::GetPixelType()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			return m_buffer->GetPixelType();

		return m_texture->GetPixelType();
	}

	void FramebufferObject::FBOTexture::Bind()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			m_buffer->Bind();
		else
			m_texture->Bind();
	}
	void FramebufferObject::FBOTexture::Unbind()
	{
		assert(m_type != Type::NONE);

		if (m_type == Type::BUFFER)
			m_buffer->Unbind();
		else
			m_texture->Unbind();
	}

	FramebufferObject::FramebufferObject(
		const std::string& name,
		UINT FBO_width, UINT FBO_height,
		Color4F ClearColor
	)
		: m_name(name), m_clearColor(ClearColor)
	{
		m_size[0] = FBO_width;
		m_size[1] = FBO_height;

		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		glUtil::setGLName(glNameType::FRAMEBUFFER, m_id, "FBO_" + name);
	}

	FramebufferObject* FramebufferObject::Create(
		const std::string& name,
		UINT FBO_width, UINT FBO_height,
		Color4F ClearColor
	){
		FramebufferObject* _FBO = new FramebufferObject(name, FBO_width, FBO_height, ClearColor);

		AddToDatabase(name, _FBO);
		Message_Created(name, _FBO);

		return _FBO;
	}

	FramebufferObject::~FramebufferObject()
	{
		if (m_id != -1)
			glDeleteFramebuffers(1, &m_id);
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

			if (m_dirtyDrawBuffers)
			{
				FixCallList();
				glDrawBuffers(m_textureCount, m_attachments);
				m_dirtyDrawBuffers = false;
			}
		}
		// update bound FBO
		m_boundID = m_id;
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

	void FramebufferObject::FixCallList()
	{
		if (m_attachments != nullptr)
			delete[] m_attachments;

		m_attachments = new GLenum[m_textureCount];
		for (GLuint i = 0; i < m_textureCount; i++)
			m_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	void FramebufferObject::addTexture(
		const std::string& name,
		Format_Type FormatType
	)
	{
		assert(m_textureCount < (GLuint)glUtil::GetMaxFBOColorAttachments());
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		// Create new render texture
		m_textures.push_back(FBOTexture());
		m_textures[m_textureCount].BecomeRenderTexture(m_size[0], m_size[1], FormatType);
		//RenderTexture* _tex = new RenderTexture(m_size[0], m_size[1], FormatType);
		//m_textures.push_back(_tex);

		// Name
		glUtil::setGLName(glNameType::TEXTURE, m_textures[m_textureCount].GetID(), "FBO_" + m_name + "_Tex_" + name);

		// Add to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_textureCount++), GL_TEXTURE_2D, m_textures[m_textureCount].GetID(), 0);
		checkFBOStatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// fix draw buffer
		m_dirtyDrawBuffers = true;
	}
	void FramebufferObject::addDepth(DepthFormat_Type depthFormatType, bool isTexture)
	{
		// Select Pixel/Channel Type [for glReadPixels]
		Pixel_Type PixelType;
		Channel_Type ChannelType;
		glUtil::getPixelChannelData(depthFormatType, ChannelType, PixelType);

		if (isTexture)
			m_depth.BecomeRenderTexture(m_size[0], m_size[1], (Format_Type)depthFormatType, ChannelType, PixelType);
		else
			m_depth.BecomeRenderBuffer(m_size[0], m_size[1], (Format_Type)depthFormatType, ChannelType, PixelType);

		// Name
		glUtil::setGLName(glNameType::TEXTURE, m_depth.GetID(), "FBO_" + m_name + "_Depth");

		// Add to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		m_depth.Bind();

		if (isTexture)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth.GetID(), 0);
		else
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth.GetID());

		checkFBOStatus();
		m_depth.Unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void FramebufferObject::bind()
	{
		bindFBO();

		glUtil::viewport(0, 0, m_size[0], m_size[1]);
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
		assert(index < (int)m_textureCount);

		glUtil::setActiveTexture(layer);

		m_textures[index].Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		assert(!m_depth.NotUsed());

		glUtil::setActiveTexture(layer);
		m_depth.Bind();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	GLubyte* FramebufferObject::readPixels(u_int textureIndex, int x, int y, int w, int h)
	{
		assert(textureIndex < m_textureCount);
		auto Texture = m_textures[textureIndex];
		
		glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
		GLubyte* data = new GLubyte[w * h * Texture.GetChannelCount()];
		glReadPixels(
			x, y, w, h,
			(GLenum)Texture.GetChannelType(),
			(GLenum)Texture.GetPixelType(),
			data
		);
		return data;
	}
	GLubyte* FramebufferObject::readDepthPixel(int x, int y, int w, int h)
	{
		//RawArray<GLubyte>* data = new RawArray<GLubyte>();
		//data->Allocate(w * h * m_depth.GetChannelCount());

		//GLubyte* data = new GLubyte[w * h * m_depth.GetChannelCount()];

		RawArray<GLubyte> asd;
		asd.start = new GLubyte[w * h * m_depth.GetChannelCount()];

		//std::vector<GLubyte> data;
		//data.reserve(w * h * m_depth.GetChannelCount() * 1);
		//data.resize(w * h * m_depth.GetChannelCount() * 1);
		//GLubyte* data = new GLubyte[w * h * m_depth.GetChannelCount()];

		glReadPixels(
			x, y, w, h,
			(GLenum)m_depth.GetChannelType(), //GL_DEPTH_COMPONENT,
			(GLenum)m_depth.GetPixelType(),
			asd.start
		);
		//std::vector<GLubyte> test = std::vector<GLubyte>(data, data + w * h * m_depth.GetChannelCount());
		return asd.start;
	}
}
