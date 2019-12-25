// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "RenderTexture.h"

namespace Vxl
{
	RenderTexture::RenderTexture(
		const std::string& name,
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType,
		bool MipMapping
	)
		: BaseTexture(TextureType::TEX_2D, TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			FormatType, Graphics::GetChannelType(FormatType), PixelType, AnisotropicMode::NONE, MipMapping)
	{
		m_name = name;
		m_width = Width;
		m_height = Height;

		// Storage
		createStorage();
		unbind();
	}

	// Utility
	void RenderTexture::RecreateStorage(uint32_t width, uint32_t height, TextureFormat format, TexturePixelType pixelType)
	{
		// Texture is immutable, destroy it and create a new one
		unbind();
		{
			auto newID = Graphics::Texture::Create();
			Graphics::Texture::Delete(m_id);
			m_id = newID;
		}
		bind();

		// Fix values
		m_width = (int)width;
		m_height = (int)height;
		m_formatType = format;
		m_pixelType = pixelType;
		
		// Update gl values
		updateParameters();
		createStorage();
		unbind();
	}
	void RenderTexture::UpdateMipmapping()
	{
		VXL_ASSERT(m_mipMapping, "Cannot update RenderTexture mipmaps because it needs to be created with the mipmap flag");
		BaseTexture::updateMipmapping();
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
		: RenderTexture("Depth", Width, Height, Graphics::GetFormat(FormatType), Graphics::GetPixelData(FormatType)), m_depthFormat(FormatType)
	{}
}