// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GlobalData.h"

#include "../textures/Texture2D.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	void GlobalData::InitGLResources()
	{
		// Editor Textures
		texID_editor_camera = GlobalAssets.loadTexture2D(
			"./assets/textures/editor_camera.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGBA8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);
		tex_editor_camera = GlobalAssets.getTexture2D(texID_editor_camera);

		//
		texID_editor_light = GlobalAssets.loadTexture2D(
			"./assets/textures/editor_lightbulb.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGBA8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);
		tex_editor_light = GlobalAssets.getTexture2D(texID_editor_light);

		// Debug Texture
		std::vector<Color3F> pixels;
		pixels.reserve(16);

		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);

		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);

		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);

		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F::BLACK);
		pixels.push_back(Color3F::PURPLE);

		texID_nullImage = GlobalAssets.createTexture2D(
			pixels, 4, true,
			TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			TextureFormat::RGB8, TextureChannelType::RGB, TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);
		tex_nullImage = GlobalAssets.getTexture2D(texID_nullImage);
	}
}