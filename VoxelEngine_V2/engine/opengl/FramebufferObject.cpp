// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"

#include "Texture.h"
#include "../utilities/logger.h"

#include <assert.h>

namespace Vxl
{
	Database<FramebufferObject> FramebufferObject::m_database;

	FramebufferObject::FramebufferObject(const std::string& name, UINT viewportWidth, UINT viewportHeight)
		: m_name(name)
	{
		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		glUtil::setOpenGLName(glNameType::FRAMEBUFFER, m_id, "FBO_" + name);
		setViewport(0, 0, viewportWidth, viewportHeight);
	}

	FramebufferObject* FramebufferObject::Create(const std::string& name, UINT viewportWidth, UINT viewportHeight)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Framebuffer: " + name);
			return nullptr;
		}

		FramebufferObject* FBO = new FramebufferObject(name, viewportWidth, viewportHeight);

		Logger.log("Created FBO: " + name);
		m_database.Set(name, FBO);

		return FBO;
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
		int Width, int Height,
		Wrap_Mode WrapMode,
		Filter_Min MinFilter,
		Filter_Mag MagFilter,
		Format_Type FormatType,
		Channel_Type ChannelType,
		Data_Type DataType
	)
	{
		assert(m_textureCount < (GLuint)glUtil::GetMaxFBOColorAttachments());

		// Create new render texture
		RenderTexture* _tex = new RenderTexture(Width, Height, WrapMode, MinFilter, MagFilter, FormatType, ChannelType, DataType);
		m_textures.push_back(_tex);
	
		// Name
		glUtil::setOpenGLName(glNameType::TEXTURE, _tex->GetID(), "FBO_" + m_name + "_Tex_" + name);

		// Add to FBO
		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (m_textureCount++), GL_TEXTURE_2D, _tex->GetID(), 0);
		checkFBOStatus();
		unbind();

		// fix draw buffer
		m_dirtyDrawBuffers = true;
	}
	void FramebufferObject::addDepth(UINT width, UINT height)
	{
		if (m_depth)
			delete m_depth;

		// Save Texture
		m_depth = new RenderTexture(width, height, Wrap_Mode::CLAMP_STRETCH, Filter_Min::NEAREST, Filter_Mag::NEAREST, Format_Type::DEPTH, Channel_Type::DEPTH, Data_Type::FLOAT);

		// Name
		glUtil::setOpenGLName(glNameType::TEXTURE, m_depth->GetID(), "FBO_" + m_name + "_Depth");

		// Add to FBO
		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->GetID(), 0);
		checkFBOStatus();
		unbind();
	}

	void FramebufferObject::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		if (m_dirtyDrawBuffers)
		{
			FixCallList();
			glDrawBuffers(m_textureCount, m_attachments);
			m_dirtyDrawBuffers = false;
		}

		glUtil::clearBuffer();
		glUtil::clearColor(m_clearColor);
	}
	void FramebufferObject::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FramebufferObject::bindViewport()
	{
		glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	}

	void FramebufferObject::bindTexture(int index, Active_Texture layer)
	{
		assert(index < (int)m_textureCount);

		glActiveTexture((GLenum)layer);
		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		assert(m_depth != nullptr);

		glActiveTexture((GLenum)layer);
		m_depth->Bind();
	}
}
