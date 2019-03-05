// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include <unordered_map>
#include <assert.h>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace Vxl
{
	class Scene;
	class Camera;

	static class Imgui_DevConsole : public Singleton<class Imgui_DevConsole>
	{
	private:
		// custom data
		static std::unordered_map<std::string, int> m_integers;
		static std::unordered_map<std::string, float> m_floats;
		static std::unordered_map<std::string, double> m_doubles;
		static std::unordered_map<std::string, Vector2> m_vec2;
		static std::unordered_map<std::string, Vector3> m_vec3;
		static std::unordered_map<std::string, Vector4> m_vec4;
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
		static float GetFloat(const std::string& str)
		{
			assert(m_floats.size() < 1000); // in case something went wrong
			return m_floats[str];
		}
		static double GetDouble(const std::string& str)
		{
			assert(m_doubles.size() < 1000); // in case something went wrong
			return m_doubles[str];
		}
		static Vector2 GetVec2(const std::string& str)
		{
			assert(m_vec2.size() < 1000); // in case something went wrong
			return m_vec2[str];
		}
		static Vector3 GetVec3(const std::string& str)
		{
			assert(m_vec3.size() < 1000); // in case something went wrong
			return m_vec3[str];
		}
		static Vector4 GetVec4(const std::string& str)
		{
			assert(m_vec4.size() < 1000); // in case something went wrong
			return m_vec4[str];
		}

		// Draw
		void Draw(Scene* scene);

	} SingletonInstance(Imgui_DevConsole);
}
