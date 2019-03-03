// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include <unordered_map>

namespace Vxl
{
	class Scene;
	class Camera;

	static class Imgui_DevConsole : public Singleton<class Imgui_DevConsole>
	{
	private:
		// custom data
		static std::unordered_map<std::string, int> m_integers;
	public:
		// Data
		bool GBUFFER_WIREFRAME = false;
		bool SHOW_NORMALS = false;
		bool SHOW_DEPTH = false;
		Camera* MAIN_CAMERA = nullptr;
		float CAMFOV = 0;
		float CAM_ZNEAR = 0;
		float CAM_ZFAR = 0;

		// custom data
		static int GetInt(const std::string& str)
		{
			assert(m_integers.size() < 1000); // in case something went wrong
			return m_integers[str];
		}

		// Const
		//static const CommonResolutions

		// Draw
		void Draw(Scene* scene);

	} SingletonInstance(Imgui_DevConsole);
}
