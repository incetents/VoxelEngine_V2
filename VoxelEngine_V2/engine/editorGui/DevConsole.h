// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

#include "../math/Vector.h"
#include "../math/Color.h"
#include "../math/MathCore.h"

#include <map>
#include <utility>

namespace Vxl
{
	class Scene;
	class Camera;

#define DEV_CONSOLE_MAX_ITEM_SIZE 1000

	static class DevConsole : public Singleton<class DevConsole>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
	private:
		// custom data
		std::map<std::string, bool> m_bools;
		std::map<std::string, int> m_integers;
		std::map<std::string, float> m_floats;
		std::map<std::string, double> m_doubles;
		std::map<std::string, Vector2> m_vec2;
		std::map<std::string, Vector3> m_vec3;
		std::map<std::string, Vector4> m_vec4;
		std::map<std::string, Color3F> m_color3;
		std::map<std::string, Color4F> m_color4;

		std::map<std::string, Vector3i> m_integersRanged;
		std::map<std::string, Vector3> m_floatsRanged;
		std::map<std::string, Vector3d> m_doublesRanged;

		std::map<std::string, std::pair<bool, std::string>> m_display_values;

		// Menu Mode
		enum class MenuState
		{
			MASTER,
			CUSTOM_VALUES,
			STATISTICS
		};
		MenuState m_State = MenuState::MASTER;
		// Draw Menu Section
		void Draw_Master(Scene* scene);
		void Draw_CustomValues();
		void Draw_Statistics();
	public:

		// custom data
		bool	GetBool(const std::string& str, bool _default)
		{
			if (m_bools.find(str) == m_bools.end())
				m_bools[str] = _default;

			VXL_ASSERT(m_bools.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many bools in Custom Data"); // in case something went wrong
			return m_bools[str];
		}
		int		GetInt(const std::string& str, int _default)
		{
			if (m_integers.find(str) == m_integers.end())
				m_integers[str] = _default;

			VXL_ASSERT(m_integers.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many integers in Custom Data"); // in case something went wrong
			return m_integers[str];
		}
		float	GetFloat(const std::string& str, float _default)
		{
			if (m_floats.find(str) == m_floats.end())
				m_floats[str] = _default;

			VXL_ASSERT(m_floats.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many floats in Custom Data"); // in case something went wrong
			return m_floats[str];
		}
		double	GetDouble(const std::string& str, double _default)
		{
			if (m_doubles.find(str) == m_doubles.end())
				m_doubles[str] = _default;

			VXL_ASSERT(m_doubles.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many doubles in Custom Data"); // in case something went wrong
			return m_doubles[str];
		}
		Vector2 GetVec2(const std::string& str, Vector2 _default)
		{
			if (m_vec2.find(str) == m_vec2.end())
				m_vec2[str] = _default;

			VXL_ASSERT(m_vec2.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec2s in Custom Data"); // in case something went wrong
			return m_vec2[str];
		}
		Vector3 GetVec3(const std::string& str, Vector3 _default)
		{
			if (m_vec3.find(str) == m_vec3.end())
				m_vec3[str] = _default;

			VXL_ASSERT(m_vec3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec3s in Custom Data"); // in case something went wrong
			return m_vec3[str];
		}
		Vector4 GetVec4(const std::string& str, Vector4 _default)
		{
			if (m_vec4.find(str) == m_vec4.end())
				m_vec4[str] = _default;

			VXL_ASSERT(m_vec4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec4s in Custom Data"); // in case something went wrong
			return m_vec4[str];
		}
		Color3F GetColor3(const std::string& str, Color3F _default)
		{
			if (m_color3.find(str) == m_color3.end())
				m_color3[str] = _default;

			VXL_ASSERT(m_color3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color3s in Custom Data"); // in case something went wrong
			return m_color3[str];
		}
		Color4F GetColor4(const std::string& str, Color4F _default)
		{
			if (m_color4.find(str) == m_color4.end())
				m_color4[str] = _default;

			VXL_ASSERT(m_color4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color4s in Custom Data"); // in case something went wrong
			return m_color4[str];
		}

		int		GetIntRange(const std::string& str, int _default, int _min, int _max)
		{
			if (m_integersRanged.find(str) == m_integersRanged.end())
				m_integersRanged[str] = Vector3i(_default, _min, _max);

			VXL_ASSERT(m_integersRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged integers in Custom Data"); // in case something went wrong
			return MacroClamp(m_integersRanged[str].x, _min, _max);
		}
		float	GetFloatRange(const std::string& str, float _default, float _min, float _max)
		{
			if (m_floatsRanged.find(str) == m_floatsRanged.end())
				m_floatsRanged[str] = Vector3(_default, _min, _max);

			VXL_ASSERT(m_floatsRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged floats in Custom Data"); // in case something went wrong
			return MacroClamp(m_floatsRanged[str].x, _min, _max);
		}
		double	GetDoubleRange(const std::string& str, double _default, double _min, double _max)
		{
			if (m_doublesRanged.find(str) == m_doublesRanged.end())
				m_doublesRanged[str] = Vector3d(_default, _min, _max);

			VXL_ASSERT(m_doublesRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged doubles in Custom Data"); // in case something went wrong
			return MacroClamp(m_doublesRanged[str].x, _min, _max);
		}

		// display data
		inline void ShowFloat(const std::string& name, float _value)
		{
			m_display_values[name] = std::pair<bool, std::string>(true, std::to_string(_value));
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

#define DEVCONSOLE_GET_INT_RANGE(x, y, a, b) DevConsole.GetIntRange(x, y, a, b)
#define DEVCONSOLE_GET_FLOAT_RANGE(x, y, a, b) DevConsole.GetFloatRange(x, y, a, b)
#define DEVCONSOLE_GET_DOUBLE_RANGE(x, y, a, b) DevConsole.GetDoubleRange(x, y, a, b)

		// Draw
		//void Draw(Scene* scene);
		void Draw() override;
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

#define DEVCONSOLE_GET_INT_RANGE(x, y, a, b) y
#define DEVCONSOLE_GET_FLOAT_RANGE(x, y, a, b) y
#define DEVCONSOLE_GET_DOUBLE_RANGE(x, y, a, b) y

	public:
		void Draw() override {}
#endif

	} SingletonInstance(DevConsole);
}
