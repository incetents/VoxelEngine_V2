// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "FramebufferObject.h"

#include "Texture.h"
#include "../utilities/logger.h"

#include <assert.h>

namespace Vxl
{
	FramebufferObject::FramebufferObject()
	{
		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
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
		if (m_attachCallList != nullptr)
			delete[] m_attachCallList;

		m_attachCallList = new GLenum[m_texCount];
		for (GLuint i = 0; i < m_texCount; i++)
			m_attachCallList[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	void FramebufferObject::addTexture(RenderTexture* _tex)
	{
		assert(m_texCount < (GLuint)glUtil::GetMaxFBOColorAttachments());
		assert(_tex != nullptr);

		// Add to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_texCount, GL_TEXTURE_2D, _tex->GetID(), 0);
		checkFBOStatus();

		// Add to list
		m_textures.push_back(_tex);
		m_texCount++;
		FixCallList();
	}
	void FramebufferObject::addDepth(int width, int height)
	{
		if (_depth)
			delete _depth;

		// Save Texture
		_depth = new RenderTexture(width, height, Format_Type::DEPTH, Wrap_Mode::CLAMP_STRETCH, Filter_Mode_Min::NEAREST, Filter_Mode_Mag::NEAREST);

		// Add to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth->GetID(), 0);
		checkFBOStatus();
	}

	void FramebufferObject::bind()
	{
		assert(m_attachCallList != nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glDrawBuffers(m_texCount, m_attachCallList);

		glUtil::clearBuffer();
		glUtil::clearColor(m_clearColor);
	}
	void FramebufferObject::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FramebufferObject::bindTexture(int index, Active_Texture layer)
	{
		assert(index < (int)m_texCount);

		glActiveTexture((GLenum)layer);
		m_textures[index]->Bind();
	}
	void FramebufferObject::bindDepth(Active_Texture layer)
	{
		assert(_depth != nullptr);

		glActiveTexture((GLenum)layer);
		_depth->Bind();
	}
}
