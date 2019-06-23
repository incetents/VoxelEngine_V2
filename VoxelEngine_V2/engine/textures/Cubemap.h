// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

namespace Vxl
{
	class Cubemap : public BaseTexture, public Asset<Cubemap>
	{
		friend class Loader;
		friend class RenderManager;
	protected:
		UCHAR**		m_image = new UCHAR*[6];

		// Constructor
		Cubemap(
			const std::string& filePath1,
			const std::string& filePath2,
			const std::string& filePath3,
			const std::string& filePath4,
			const std::string& filePath5,
			const std::string& filePath6,
			bool			InvertY = true,
			bool			UseMipMapping = true,
			Wrap_Mode		WrapMode = Wrap_Mode::REPEAT,
			Filter_Mode		FilterMode = Filter_Mode::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE,
			Anisotropic_Mode AnisotropicMode = Anisotropic_Mode::NONE
		);
	public:

		// Load Cubemap
		static Cubemap* Load(
			const std::string& name,
			const std::string& filePath1, const std::string& filePath2, const std::string& filePath3, const std::string& filePath4, const std::string& filePath5, const std::string& filePath6,
			bool InvertY = false,
			bool UseMipMapping = true,
			Wrap_Mode WrapMode = Wrap_Mode::CLAMP_STRETCH,
			Filter_Mode		FilterMode = Filter_Mode::LINEAR,
			Format_Type		FormatType = Format_Type::RGBA8,
			Pixel_Type		PixelType = Pixel_Type::UNSIGNED_BYTE,
			Anisotropic_Mode AnisotropicMode = Anisotropic_Mode::NONE
		);

		Cubemap(const Cubemap&) = delete;
		~Cubemap();

		inline bool IsLoaded(void) const override
		{
			return m_loaded;
		}
	};
}