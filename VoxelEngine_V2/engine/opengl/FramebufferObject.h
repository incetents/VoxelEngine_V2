// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include "glUtil.h"
#include "../math/Color.h"
#include "../utilities/Database.h"

//#include "Texture.h"
#include <vector>

namespace Vxl
{
	class RenderTexture;

	class FramebufferObject
	{
	private:
		// Fbo
		const std::string m_name;
		GLuint	m_id = -1;
		Color4F m_clearColor = Color4F(0,0,0,1);
		UINT	m_size[2];
		bool	m_dirtyDrawBuffers = false;
		// Textures
		std::vector<RenderTexture*> m_textures;
		GLuint						m_textureCount = 0;
		GLenum*						m_attachments = nullptr;
		// Depth
		RenderTexture*				m_depth = nullptr;
		// Tracker //
		static GLuint m_boundID;

		// Utility
		void FixCallList();
		bool checkFBOStatus();
		void bindFBO();
		void clearColor();

		// Database
		static Database<FramebufferObject> m_database;
		// Protected Creation
		FramebufferObject(
			const std::string& name,
			UINT FBO_width, UINT FBO_height,
			Color4F ClearColor
		);
	public:
		// Creator
		static FramebufferObject* Create(
			const std::string& name,
			UINT FBO_width, UINT FBO_height,
			Color4F ClearColor
		);
		// Accessor
		static FramebufferObject* Get(const std::string& name)
		{
			return m_database.Get(name);
		}

		~FramebufferObject();

		inline void setClearColor(Color4F c)
		{
			m_clearColor = c;
		}

		void addTexture(
			const std::string& name,
			Wrap_Mode WrapMode			= Wrap_Mode::CLAMP_STRETCH,
			Filter_Min MinFilter		= Filter_Min::NEAREST,
			Filter_Mag MagFilter		= Filter_Mag::NEAREST,
			Format_Type FormatType		= Format_Type::RGBA,
			Channel_Type ChannelType	= Channel_Type::RGBA,
			Data_Type DataType			= Data_Type::UNSIGNED_BYTE
		);
		void addDepth();

		void bind();
		void bind(UINT viewportX, UINT viewportY, UINT viewportW, UINT viewportH);
		static void unbind();

		void bindTexture(int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);
	};
}