// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class Scene;
	class Camera;

	static class Imgui_DevConsole : public Singleton<class Imgui_DevConsole>
	{
	public:
		// Data
		bool GBUFFER_WIREFRAME = false;
		bool SHOW_NORMALS = false;
		bool SHOW_DEPTH = false;
		Camera* MAIN_CAMERA = nullptr;
		float CAMFOV = 0;
		float CAM_ZNEAR = 0;
		float CAM_ZFAR = 0;

		// Const
		//static const CommonResolutions

		// Draw
		void Draw(Scene* scene);

	} SingletonInstance(Imgui_DevConsole);
}
