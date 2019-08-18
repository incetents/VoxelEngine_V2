// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Graphics.h"

#include "../math/Color.h"
#include "../utilities/Asset.h"
#include "../utilities/Util.h"
#include "../utilities/Containers.h"
#include "../utilities/Macros.h"

#include <vector>
#include <Windows.h>

namespace Vxl
{
	class RenderBuffer;
	class RenderTexture;


	//	class FramebufferAttachment
	//	{
	//		friend class FramebufferObject;
	//	private:
	//		// Data
	//		const AttachmentType	m_type;
	//		bool					m_empty = true;
	//		bool					m_isReference = false; // Whether Attachment is being read from somewhere else
	//	
	//		// Attachment Info
	//		const std::string	parent_name;
	//		const std::string	attachment_name;
	//		TextureFormat		attachment_formatType;
	//		TexturePixelType	attachment_pixelType;
	//	
	//		// Attachment Data [ Can only be one object ]
	//		union
	//		{
	//			RenderTexture* m_renderTexture;
	//			RenderBuffer*  m_renderBuffer;
	//		};
	//	
	//	public:
	//		FramebufferAttachment(
	//			const std::string& Parentname,
	//			const std::string& name,
	//			AttachmentType type,
	//			TextureFormat formatType,
	//			TexturePixelType pixelType
	//		)
	//			: parent_name(Parentname), attachment_name(name), m_type(type), attachment_formatType(formatType), attachment_pixelType(pixelType)
	//		{}
	//		FramebufferAttachment(
	//			const std::string& Parentname,
	//			const std::string& name,
	//			RenderTexture* renderTexture
	//		)
	//			: parent_name(Parentname), attachment_name(name), m_type(AttachmentType::TEXTURE)
	//		{
	//			m_renderTexture = renderTexture;
	//			m_isReference = true;
	//		}
	//		~FramebufferAttachment()
	//		{
	//			unload();
	//		}
	//	
	//		void load(int Width, int Height);
	//		void unload();
	//	
	//		inline bool isRenderTexture(void) const
	//		{
	//			return m_type == AttachmentType::TEXTURE;
	//		}
	//		inline bool isRenderBuffer(void) const
	//		{
	//			return m_type == AttachmentType::BUFFER;
	//		}
	//	
	//		uint32_t			GetID(void) const;
	//		TextureFormat		GetFormatType(void) const;
	//		TextureChannelType	GetChannelType(void) const;
	//		TexturePixelType	GetPixelType(void) const;
	//		int					GetChannelCount(void) const;
	//	
	//		void Bind();
	//		void Unbind();
	//	};

	

	class FBOAttachment
	{
	public:
		// Type
		enum class Type
		{
			// ERROR
			NONE = 0,
			// Render Buffer [Cannot bind like a texture]
			BUFFER,
			// Render Texture
			TEXTURE
		};

	private:
		// Data
		Type m_type = Type::NONE;

		union // [ Can only be one object ]
		{
			RenderTexture* m_renderTexture;
			RenderBuffer*  m_renderBuffer;
		};

	public:
		void Set(RenderTexture* _renderTexture)
		{
			m_type = Type::TEXTURE;
			m_renderTexture = _renderTexture;
		}
		void Set(RenderBuffer* _renderBuffer)
		{
			m_type = Type::BUFFER;
			m_renderBuffer = _renderBuffer;
		}
		void Remove()
		{
			m_type = Type::NONE;
			m_renderTexture = nullptr;
		}

		RenderTexture* GetRenderTexture(void) const
		{
			VXL_ASSERT(m_type == Type::TEXTURE, "Incorrect FBO Attachment Type for Getter");
			return m_renderTexture;
		}
		RenderBuffer* GetRenderBuffer(void) const
		{
			VXL_ASSERT(m_type == Type::BUFFER, "Incorrect FBO Attachment Type for Getter");
			return m_renderBuffer;
		}

		TextureFormat GetFormatType(void) const;

		Type GetType(void) const
		{
			return m_type;
		}
		bool IsRenderTexture(void) const
		{
			return m_type == Type::TEXTURE;
		}
		bool IsRenderBuffer(void) const
		{
			return m_type == Type::BUFFER;
		}
		bool IsUnused(void) const
		{
			return m_type == Type::NONE;
		}
	};

	class FramebufferObject : public Asset<FramebufferObject>
	{
		friend class RenderManager;
	private:
		
		enum class ClearMode
		{
			COLOR,
			COLOR_DEPTH,
			COLOR_STENCIL,
			COLOR_DEPTH_STENCIL
		};

	private:

		// Fbo
		const std::string	m_name;
		FramebufferObjectID	m_id = -1;
		Color4F				m_clearColor = Color4F(0,0,0,0);
		ClearMode			m_clearMode = ClearMode::COLOR;
		uint32_t			m_width = 0;
		uint32_t			m_height = 0;
		bool				m_fullscreen = false; // Width/Height will attempt to match screen's resolution
		// Attachments
		mutable std::map<uint32_t, FBOAttachment>	m_textures;
		std::vector<uint32_t>				m_attachmentOrder;
		bool								m_attachmentOrderDirty = false;
		FBOAttachment						m_depth;

		void load();
		void unload();

		// Constructor
		FramebufferObject(const std::string& name);
	public:
		// Creator
		static FramebufferObject* Create(const std::string& name);

		~FramebufferObject();

		// Sizing
		inline void SetClearColor(Color4F c)
		{
			m_clearColor = c;
		}
		inline void SetSize(unsigned int Width, unsigned int Height)
		{
			m_width = Width;
			m_height = Height;
			m_fullscreen = false;
		}
		void		SetSizeToWindowSize(); // Size will copy Window's resolution

		// Utility
		bool HasAttachment(uint32_t _attachmentIndex) const
		{
			return m_textures.find(_attachmentIndex) != m_textures.end();
		}

		// Binding related
		void Bind();
		void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
		void DrawToBuffers();
		static void Unbind();

		// Clears the color from an entire Texture
		void ClearBuffers();
		void ClearBuffer(uint32_t attachmentIndex);

		// Creating Attachment Info
		RenderTexture* CreateRenderTexture(const std::string& name, TextureFormat format = TextureFormat::RGBA8) const;
		RenderBuffer* CreateRenderBuffer(const std::string& name, TextureFormat format = TextureFormat::RGBA8) const;

		// Editing Attachment Info
		void SetAttachment(
			uint32_t _attachmentIndex,
			RenderTexture* _renderTexture
		);
		void SetAttachment(
			uint32_t _attachmentIndex,
			RenderBuffer* _renderbuffer
		);
		RenderTexture* GetAttachmentRenderTexture(
			uint32_t _attachmentIndex
		);
		RenderBuffer* GetAttachmentRenderBuffer(
			uint32_t _attachmentIndex
		);
		void RemoveAttachment(
			uint32_t _attachmentIndex
		);
		// Attachment is still part of FBO class, it just doesn't get drawn to
		void DisableAttachment(
			uint32_t _attachmentIndex
		);
		void EnableAttachment(
			uint32_t _attachmentIndex
		);

		void SetDepth(
			TextureDepthFormat _depthFormat,
			FBOAttachment::Type _type
		);
		void RemoveDepth(void);

		// Utility //
		bool checkFBOStatus();

		void bindTexture(uint32_t index, TextureLevel layer);
		void bindDepth(TextureLevel layer);

		void blitColor(const FramebufferObject& destFBO, uint32_t srcAttachment, uint32_t destAttachment);
		void blitDepth(const FramebufferObject& destFBO);

		void generateMipmaps(uint32_t _attachmentIndex);

		RawArray<uint8_t> readPixels(uint32_t _attachmentIndex, int x, int y, int w, int h);
		RawArray<uint8_t> readPixelsFromMouse(uint32_t _attachmentIndex, int w, int h);

		RawArray<uint8_t> readDepthPixels(int x, int y, int w, int h);
		RawArray<uint8_t> readDepthPixelsFromMouse(int w, int h);

		// Getters
		inline uint32_t GetID(void) const
		{
			return m_id;
		}
		inline uint32_t GetWidth(void) const
		{
			return m_width;
		}
		inline uint32_t GetHeight(void) const
		{
			return m_height;
		}
	};

}