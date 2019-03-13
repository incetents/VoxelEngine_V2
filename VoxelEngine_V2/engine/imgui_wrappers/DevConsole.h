// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include <unordered_map>
#include <assert.h>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Color.h"

namespace Vxl
{
	class Scene;
	class Camera;

#define DEV_CONSOLE_MAX_ITEM_SIZE 1000

	static class DevConsole : public Singleton<class DevConsole>
	{
	private:
		// custom data
		std::unordered_map<std::string, bool> m_bools;
		std::unordered_map<std::string, int> m_integers;
		std::unordered_map<std::string, float> m_floats;
		std::unordered_map<std::string, double> m_doubles;
		std::unordered_map<std::string, Vector2> m_vec2;
		std::unordered_map<std::string, Vector3> m_vec3;
		std::unordered_map<std::string, Vector4> m_vec4;
		std::unordered_map<std::string, Color3F> m_color3;
		std::unordered_map<std::string, Color4F> m_color4;
		
		// Menu Mode

	public:
		// Data
		//bool GBUFFER_WIREFRAME = false;
		bool SHOW_NORMALS = false;
		bool SHOW_DEPTH = false;
		Camera* MAIN_CAMERA = nullptr;
		float CAMFOV = 0;
		float CAM_ZNEAR = 0;
		float CAM_ZFAR = 0;

		// custom data
		static bool GetBool(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_bools.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_bools[str];
		}
		static int GetInt(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_integers.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_integers[str];
		}
		static float GetFloat(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_floats.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_floats[str];
		}
		static double GetDouble(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_doubles.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_doubles[str];
		}
		static Vector2 GetVec2(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_vec2.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_vec2[str];
		}
		static Vector3 GetVec3(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_vec3.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_vec3[str];
		}
		static Vector4 GetVec4(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_vec4.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_vec4[str];
		}
		static Color3F GetColor3(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_color3.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_color3[str];
		}
		static Color4F GetColor4(const std::string& str)
		{
			assert(DevConsole::instanceRef.m_color4.size() < DEV_CONSOLE_MAX_ITEM_SIZE); // in case something went wrong
			return DevConsole::instanceRef.m_color4[str];
		}

		// Draw
		void Draw(Scene* scene);

	} SingletonInstance(DevConsole);
}
