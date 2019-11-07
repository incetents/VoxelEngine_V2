// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Asset.h"

namespace Vxl
{
	static class GlobalData : public Singleton<class GlobalData>
	{
		friend class RenderManager;
	private:
		void InitGLResources();

		// Textures
		Texture2DIndex texID_nullImage;
		Texture2DIndex texID_editor_camera;
		Texture2DIndex texID_editor_light;

	public:
		Texture2D* tex_nullImage = nullptr;
		Texture2D* tex_editor_camera = nullptr;
		Texture2D* tex_editor_light = nullptr;

	} SingletonInstance(GlobalData);
}