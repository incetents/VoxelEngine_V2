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
		bool	m_dirtyDrawBuffers = false;
		UINT	m_viewport[4];
		// Textures
		std::vector<RenderTexture*> m_textures;
		GLuint						m_textureCount = 0;
		GLenum*						m_attachments = nullptr;
		// Depth
		RenderTexture*				m_depth = nullptr;

		void FixCallList();
		bool checkFBOStatus();

		// Database
		static Database<FramebufferObject> m_database;
		// Protected Creation
		FramebufferObject(const std::string& name, UINT viewportWidth, UINT viewportHeight);
	public:
		// Creator
		static FramebufferObject* Create(const std::string& name, UINT viewportWidth, UINT viewportHeight);
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
		inline void setViewport(
			UINT viewportOffsetx, UINT viewportOffsety,
			UINT viewportWidth, UINT viewportHeight
		)
		{
			m_viewport[0] = viewportOffsetx;
			m_viewport[1] = viewportOffsety;
			m_viewport[2] = viewportWidth;
			m_viewport[3] = viewportHeight;
		}

		void addTexture(
			const std::string& name,
			int Width, int Height,
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Min MinFilter = Filter_Min::NEAREST,
			Filter_Mag MagFilter = Filter_Mag::NEAREST,
			Format_Type FormatType = Format_Type::RGBA,
			Channel_Type ChannelType = Channel_Type::RGBA,
			Data_Type DataType = Data_Type::UNSIGNED_BYTE
		);
		void addDepth(UINT width, UINT height);

		void bind();
		static void unbind();

		void bindViewport();

		void bindTexture(int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);
	};
}