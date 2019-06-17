// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"
#include <unordered_map>

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
#ifdef GLOBAL_IMGUI
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
		enum class MenuState
		{
			MASTER,
			CUSTOM_VALUES
		};
		MenuState m_State = MenuState::MASTER;
		// Draw Menu Section
		void Draw_Master(Scene* scene);
		void Draw_CustomValues();
	public:

		// custom data
		bool	GetBool(const std::string& str, bool _default)
		{
			if (DevConsole::instanceRef.m_bools.find(str) == DevConsole::instanceRef.m_bools.end())
				DevConsole::instanceRef.m_bools[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_bools.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many bools in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_bools[str];
		}
		int		GetInt(const std::string& str, int _default)
		{
			if (DevConsole::instanceRef.m_integers.find(str) == DevConsole::instanceRef.m_integers.end())
				DevConsole::instanceRef.m_integers[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_integers.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many integers in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_integers[str];
		}
		float	GetFloat(const std::string& str, float _default)
		{
			if (DevConsole::instanceRef.m_floats.find(str) == DevConsole::instanceRef.m_floats.end())
				DevConsole::instanceRef.m_floats[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_floats.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many floats in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_floats[str];
		}
		double	GetDouble(const std::string& str, double _default)
		{
			if (DevConsole::instanceRef.m_doubles.find(str) == DevConsole::instanceRef.m_doubles.end())
				DevConsole::instanceRef.m_doubles[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_doubles.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many doubles in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_doubles[str];
		}
		Vector2 GetVec2(const std::string& str, Vector2 _default)
		{
			if (DevConsole::instanceRef.m_vec2.find(str) == DevConsole::instanceRef.m_vec2.end())
				DevConsole::instanceRef.m_vec2[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_vec2.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec2s in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_vec2[str];
		}
		Vector3 GetVec3(const std::string& str, Vector3 _default)
		{
			if (DevConsole::instanceRef.m_vec3.find(str) == DevConsole::instanceRef.m_vec3.end())
				DevConsole::instanceRef.m_vec3[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_vec3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec3s in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_vec3[str];
		}
		Vector4 GetVec4(const std::string& str, Vector4 _default)
		{
			if (DevConsole::instanceRef.m_vec4.find(str) == DevConsole::instanceRef.m_vec4.end())
				DevConsole::instanceRef.m_vec4[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_vec4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec4s in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_vec4[str];
		}
		Color3F GetColor3(const std::string& str, Color3F _default)
		{
			if (DevConsole::instanceRef.m_color3.find(str) == DevConsole::instanceRef.m_color3.end())
				DevConsole::instanceRef.m_color3[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_color3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color3s in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_color3[str];
		}
		Color4F GetColor4(const std::string& str, Color4F _default)
		{
			if (DevConsole::instanceRef.m_color4.find(str) == DevConsole::instanceRef.m_color4.end())
				DevConsole::instanceRef.m_color4[str] = _default;

			VXL_ASSERT(DevConsole::instanceRef.m_color4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color4s in Custom Data"); // in case something went wrong
			return DevConsole::instanceRef.m_color4[str];
		}

#define DEVCONSOLE_GET_BOOL(x, y) DevConsole.GetBool(x, y)
#define DEVCONSOLE_GET_INT(x, y) DevConsole.GetInt(x, y)
#define DEVCONSOLE_GET_FLOAT(x, y) DevConsole.GetFloat(x, y)
#define DEVCONSOLE_GET_DOUBLE(x, y) DevConsole.GetDouble(x, y)
#define DEVCONSOLE_GET_VEC2(x, y) DevConsole.GetVec2(x, y)
#define DEVCONSOLE_GET_VEC3(x, y) DevConsole.GetVec3(x, y)
#define DEVCONSOLE_GET_VEC4(x, y) DevConsole.GetVec4(x, y)
#define DEVCONSOLE_GET_COLOR3(x, y) DevConsole.GetColor3(x, y)
#define DEVCONSOLE_GET_COLOR4(x, y) DevConsole.GetColor4(x, y)

		// Draw
		void Draw(Scene* scene);
#else

#define DEVCONSOLE_GET_BOOL(x, y) y
#define DEVCONSOLE_GET_INT(x, y) y
#define DEVCONSOLE_GET_FLOAT(x, y) y
#define DEVCONSOLE_GET_DOUBLE(x, y) y
#define DEVCONSOLE_GET_VEC2(x, y) y
#define DEVCONSOLE_GET_VEC3(x, y) y
#define DEVCONSOLE_GET_VEC4(x, y) y
#define DEVCONSOLE_GET_COLOR3(x, y) y
#define DEVCONSOLE_GET_COLOR4(x, y) y

	public:
		void Draw(Scene* scene) {}
#endif

	} SingletonInstance(DevConsole);
}
