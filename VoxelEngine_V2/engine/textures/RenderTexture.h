// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class RenderTexture : public BaseTexture
	{
		DISALLOW_COPY_AND_ASSIGN(RenderTexture);
		friend class _Assets;
	protected:
		// Name
		std::string m_name;

		// Channel/Pixel used only for reading
		RenderTexture(
			const std::string& name,
			int Width,
			int Height,
			TextureFormat FormatType = TextureFormat::RGBA8,
			TexturePixelType PixelType = TexturePixelType::UNSIGNED_BYTE,
			bool MipMapping = false
		);

	public:

		// Utility
		void RecreateStorage(uint32_t width, uint32_t height, TextureFormat format, TexturePixelType pixelType);
		void UpdateMipmapping();
		void setGLName(const std::string& name);
		
		// Getters
		inline std::string getName(void) const
		{
			return m_name;
		}
	};

	class RenderTextureDepth : public RenderTexture
	{
		DISALLOW_COPY_AND_ASSIGN(RenderTextureDepth);
		friend class _Assets;
	protected:
		// Created using only depth format
		RenderTextureDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);

	public:
		// Additional Data
		const TextureDepthFormat m_depthFormat;
	};
}