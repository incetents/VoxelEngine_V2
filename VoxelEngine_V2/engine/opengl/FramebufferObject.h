// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include "glUtil.h"
#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"

//#include "Texture.h"
#include <vector>

namespace Vxl
{
	class RenderBuffer;
	class RenderTexture;

	class FramebufferObject : public Asset<FramebufferObject>
	{
		friend class RenderManager;
	private:
		class FBOTexture
		{
			enum Type
			{
				NONE = -1,
				TEXTURE,
				BUFFER
			};
			Type m_type = Type::NONE;
		public:
			union
			{
				RenderTexture* m_texture;
				RenderBuffer* m_buffer;
			};

			void BecomeRenderTexture(
				int Width, int Height,
				Format_Type FormatType,
				Channel_Type ChannelType = Channel_Type::RGBA,
				Pixel_Type PixelType = Pixel_Type::UNSIGNED_BYTE
			);
			void BecomeRenderBuffer(
				int Width, int Height,
				Format_Type FormatType,
				Channel_Type ChannelType = Channel_Type::RGBA,
				Pixel_Type PixelType = Pixel_Type::UNSIGNED_BYTE
			);

			inline bool NotUsed(void) const
			{
				return m_type == Type::NONE;
			}
			
			GLuint GetID();
			int GetChannelCount();
			Channel_Type GetChannelType();
			Pixel_Type GetPixelType();

			void Bind();
			void Unbind();
		};
	private:
		// Fbo
		const std::string m_name;
		GLuint	m_id = -1;
		Color4F m_clearColor = Color4F(0,0,0,1);
		UINT    m_clearBuffers = (GLbitfield)Buffer_Bit_Type::COLOR | (GLbitfield)Buffer_Bit_Type::DEPTH;
		UINT	m_size[2];
		bool	m_dirtyDrawBuffers = false;
		// Textures
		std::vector<FBOTexture>		m_textures;
		//std::vector<RenderTexture*> m_textures;
		GLuint						m_textureCount = 0;
		GLenum*						m_attachments = nullptr;
		// Depth
		FBOTexture m_depth;
		//RenderTexture*				m_depth = nullptr;
		// Tracker //
		static GLuint m_boundID;

		// Utility
		void FixCallList();
		bool checkFBOStatus();
		void bindFBO();
		
		// Constructor
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

		~FramebufferObject();

		inline GLuint GetID(void) const
		{
			return m_id;
		}
		inline GLuint GetWidth(void) const
		{
			return m_size[0];
		}
		inline GLuint GetHeight(void) const
		{
			return m_size[1];
		}

		inline void setClearColor(Color4F c)
		{
			m_clearColor = c;
		}
		inline void setClearBuffer(Buffer_Bit_Type b1, Buffer_Bit_Type b2 = Buffer_Bit_Type::NONE, Buffer_Bit_Type b3 = Buffer_Bit_Type::NONE, Buffer_Bit_Type b4 = Buffer_Bit_Type::NONE)
		{
			m_clearBuffers = (GLbitfield)b1 | (GLbitfield)b2 | (GLbitfield)b3 | (GLbitfield)b4;
		}

		// Clears the color from an entire Texture
		void clearBuffers();
		void clearBuffer(unsigned int index);

		void addTexture(
			const std::string& name,
			Format_Type FormatType		= Format_Type::RGBA8
		);
		void addDepth(
			DepthFormat_Type depthFormatType,
			bool isTexture = true
		);

		void bind();
		void bind(UINT viewportX, UINT viewportY, UINT viewportW, UINT viewportH);
		static void unbind();

		void bindTexture(int index, Active_Texture layer);
		void bindDepth(Active_Texture layer);

		GLubyte* readPixels(u_int textureIndex, int x, int y, int w, int h);
		GLubyte* readDepthPixel(int x, int y, int w, int h);
	};
}