// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType,
		bool MipMapping
	)
		: BaseTexture(TextureType::TEX_2D, TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			FormatType, Graphics::GetChannelType(FormatType), PixelType, AnisotropicMode::NONE, MipMapping)
	{
		m_width = Width;
		m_height = Height;

		// Storage
		CreateStorage();
		Unbind();
	}

	// Utility
	void RenderTexture::RecreateStorage(uint32_t width, uint32_t height, TextureFormat format, TexturePixelType pixelType)
	{
		// Texture is immutable, destroy it and create a new one
		Unbind();
		{
			auto newID = Graphics::Texture::Create();
			Graphics::Texture::Delete(m_id);
			m_id = newID;
		}
		Bind();

		// Fix values
		m_width = (int)width;
		m_height = (int)height;
		m_formatType = format;
		m_pixelType = pixelType;
		
		// Update gl values
		UpdateParameters();
		CreateStorage();
		Unbind();
	}
	void RenderTexture::UpdateMipmapping()
	{
		VXL_ASSERT(m_mipMapping, "Cannot update RenderTexture mipmaps because it needs to be created with the mipmap flag");
		BaseTexture::UpdateMipmapping();
	}
	void RenderTexture::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::TEXTURE, m_id, "RenderTexture_" + name);
	}

	RenderTextureDepth::RenderTextureDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
		: RenderTexture(Width, Height, Graphics::GetFormat(FormatType), Graphics::GetPixelData(FormatType)), m_depthFormat(FormatType)
	{}
}