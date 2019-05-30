// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"

#include "Texture.h"
#include "../utilities/logger.h"

#include <assert.h>

namespace Vxl
{
	GLuint FramebufferObject::m_boundID = 0;

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
		Wrap_Mode WrapMode,
		Min_Filter MinFilter,
		Mag_Filter MagFilter,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Data_Type DataType
	)
	{
		assert(m_textureCount < (GLuint)glUtil::GetMaxFBOColorAttachments());

		// Create new render texture
		RenderTexture* _tex = new RenderTexture(m_size[0], m_size[1], WrapMode, MinFilter, MagFilter, FormatType, ChannelType, DataType);
		m_textures.push_back(_tex);

		// Name
		glUtil::setGLName(glNameType::TEXTURE, _tex->GetID(), "FBO_" + m_name + "_Tex_" + name);

		// Add to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_textureCount++), GL_TEXTURE_2D, _tex->GetID(), 0);
		checkFBOStatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// fix draw buffer
		m_dirtyDrawBuffers = true;
	}
	void FramebufferObject::addDepth()
	{
		if (m_depth)
			delete m_depth;

		// Save Texture
		m_depth = new RenderTexture(m_size[0], m_size[1], Wrap_Mode::CLAMP_STRETCH, Min_Filter::NEAREST, Mag_Filter::NEAREST, Format_Type::DEPTH, Channel_Type::DEPTH, Data_Type::FLOAT);

		// Name
		glUtil::setGLName(glNameType::TEXTURE, m_depth->GetID(), "FBO_" + m_name + "_Depth");

		// Add to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->GetID(), 0);
		checkFBOStatus();
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
		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		assert(m_depth != nullptr);

		glUtil::setActiveTexture(layer);
		m_depth->Bind();
	}

	// Notice, SNORM TEXTURES CANNOT BE READ
	GLubyte* FramebufferObject::readPixels(u_int textureIndex, int x, int y, int w, int h)
	{
		assert(textureIndex < m_textureCount);
		auto Texture = m_textures[textureIndex];

		glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
		GLubyte* data = new GLubyte[w * h * Texture->GetChannelCount()];
		glReadPixels(
			x, y, w, h,
			(GLenum)Texture->GetFormatType(),
			(GLenum)Data_Type::UNSIGNED_BYTE,
			data
		);
		return data;
	}
}
