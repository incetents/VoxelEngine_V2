// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"
#include "../math/Color.h"

//#include "Texture.h"
#include <vector>

namespace Vxl
{
	class RenderTexture;

	class FramebufferObject
	{
	private:
		// Fbo
		GLuint m_id = -1;
		Color4F m_clearColor = Color4F(0,0,0,1);
		// Textures
		GLuint m_texCount = 0;
		std::vector<RenderTexture*> m_textures;
		GLenum* m_attachCallList = nullptr;
		// Depth
		RenderTexture* _depth = nullptr;

		void FixCallList();
		bool checkFBOStatus();
	public:
		FramebufferObject();
		~FramebufferObject();

		inline void setClearColor(Color4F c)
		{
			m_clearColor = c;
		}

		void addTexture(RenderTexture* _tex);
		void addDepth(int width, int height);

		void bind();
		static void unbind();

		void bindTexture(int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);
	};
}