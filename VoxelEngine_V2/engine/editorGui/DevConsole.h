// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

#include "../math/Vector.h"
#include "../math/Color.h"
#include "../math/MathCore.h"

#include "../rendering/Graphics.h"

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
		struct EditData
		{
			std::map<std::string, bool>		m_bools;
			std::map<std::string, int>		m_integers;
			std::map<std::string, float>	m_floats;
			std::map<std::string, double>	m_doubles;
			std::map<std::string, Vector2>	m_vec2;
			std::map<std::string, Vector3>	m_vec3;
			std::map<std::string, Vector4>	m_vec4;
			std::map<std::string, Color3F>	m_color3;
			std::map<std::string, Color4F>	m_color4;

			std::map<std::string, Vector3i> m_integersRanged;
			std::map<std::string, Vector3>  m_floatsRanged;
			std::map<std::string, Vector3d> m_doublesRanged;
		};
		EditData m_editData;
		
		struct GenericData
		{
			std::shared_ptr<void> data;
			GenericDataType type;

			template<typename Type>
			GenericData(GenericDataType _type, Type _data)
				: type(_type)
			{
				data = std::make_shared<Type>(_data);
			}
			GenericData() {}

			template<typename Type>
			Type* GetData() const
			{
				return static_cast<Type*>(data.get());
			}
		};
		std::map<std::string, GenericData> m_showData;
		
		// Utility 
		template<typename Type>
		void Write_Statistic(void* data)
		{
			
			
		}

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
			if (m_editData.m_bools.find(str) == m_editData.m_bools.end())
				m_editData.m_bools[str] = _default;

			VXL_ASSERT(m_editData.m_bools.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many bools in Custom Data"); // in case something went wrong
			return m_editData.m_bools[str];
		}
		int		GetInt(const std::string& str, int _default)
		{
			if (m_editData.m_integers.find(str) == m_editData.m_integers.end())
				m_editData.m_integers[str] = _default;

			VXL_ASSERT(m_editData.m_integers.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many integers in Custom Data"); // in case something went wrong
			return m_editData.m_integers[str];
		}
		float	GetFloat(const std::string& str, float _default)
		{
			if (m_editData.m_floats.find(str) == m_editData.m_floats.end())
				m_editData.m_floats[str] = _default;

			VXL_ASSERT(m_editData.m_floats.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many floats in Custom Data"); // in case something went wrong
			return m_editData.m_floats[str];
		}
		double	GetDouble(const std::string& str, double _default)
		{
			if (m_editData.m_doubles.find(str) == m_editData.m_doubles.end())
				m_editData.m_doubles[str] = _default;

			VXL_ASSERT(m_editData.m_doubles.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many doubles in Custom Data"); // in case something went wrong
			return m_editData.m_doubles[str];
		}
		Vector2 GetVec2(const std::string& str, Vector2 _default)
		{
			if (m_editData.m_vec2.find(str) == m_editData.m_vec2.end())
				m_editData.m_vec2[str] = _default;

			VXL_ASSERT(m_editData.m_vec2.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec2s in Custom Data"); // in case something went wrong
			return m_editData.m_vec2[str];
		}
		Vector3 GetVec3(const std::string& str, Vector3 _default)
		{
			if (m_editData.m_vec3.find(str) == m_editData.m_vec3.end())
				m_editData.m_vec3[str] = _default;

			VXL_ASSERT(m_editData.m_vec3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec3s in Custom Data"); // in case something went wrong
			return m_editData.m_vec3[str];
		}
		Vector4 GetVec4(const std::string& str, Vector4 _default)
		{
			if (m_editData.m_vec4.find(str) == m_editData.m_vec4.end())
				m_editData.m_vec4[str] = _default;

			VXL_ASSERT(m_editData.m_vec4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many vec4s in Custom Data"); // in case something went wrong
			return m_editData.m_vec4[str];
		}
		Color3F GetColor3(const std::string& str, Color3F _default)
		{
			if (m_editData.m_color3.find(str) == m_editData.m_color3.end())
				m_editData.m_color3[str] = _default;

			VXL_ASSERT(m_editData.m_color3.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color3s in Custom Data"); // in case something went wrong
			return m_editData.m_color3[str];
		}
		Color4F GetColor4(const std::string& str, Color4F _default)
		{
			if (m_editData.m_color4.find(str) == m_editData.m_color4.end())
				m_editData.m_color4[str] = _default;

			VXL_ASSERT(m_editData.m_color4.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many Color4s in Custom Data"); // in case something went wrong
			return m_editData.m_color4[str];
		}

		int		GetIntRange(const std::string& str, int _default, int _min, int _max)
		{
			if (m_editData.m_integersRanged.find(str) == m_editData.m_integersRanged.end())
				m_editData.m_integersRanged[str] = Vector3i(_default, _min, _max);

			VXL_ASSERT(m_editData.m_integersRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged integers in Custom Data"); // in case something went wrong
			return MacroClamp(m_editData.m_integersRanged[str].x, _min, _max);
		}
		float	GetFloatRange(const std::string& str, float _default, float _min, float _max)
		{
			if (m_editData.m_floatsRanged.find(str) == m_editData.m_floatsRanged.end())
				m_editData.m_floatsRanged[str] = Vector3(_default, _min, _max);

			VXL_ASSERT(m_editData.m_floatsRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged floats in Custom Data"); // in case something went wrong
			return MacroClamp(m_editData.m_floatsRanged[str].x, _min, _max);
		}
		double	GetDoubleRange(const std::string& str, double _default, double _min, double _max)
		{
			if (m_editData.m_doublesRanged.find(str) == m_editData.m_doublesRanged.end())
				m_editData.m_doublesRanged[str] = Vector3d(_default, _min, _max);

			VXL_ASSERT(m_editData.m_doublesRanged.size() < DEV_CONSOLE_MAX_ITEM_SIZE, "DevConsole has too many ranged doubles in Custom Data"); // in case something went wrong
			return MacroClamp(m_editData.m_doublesRanged[str].x, _min, _max);
		}

		// display data
		void RemoveShowData(const std::string& name)
		{
			if (m_showData.find(name) != m_showData.end())
				m_showData.erase(name);
		}

		void ShowBool(const std::string& name, bool _value)
		{
			m_showData[name] = GenericData(GenericDataType::BOOL, _value);
		}
		void ShowInt(const std::string& name, int _value)
		{
			m_showData[name] = GenericData(GenericDataType::INT, _value);
		}
		void ShowFloat(const std::string& name, float _value)
		{
			m_showData[name] = GenericData(GenericDataType::FLOAT, _value);
		}
		void ShowDouble(const std::string& name, double _value)
		{
			m_showData[name] = GenericData(GenericDataType::DOUBLE, _value);
		}
		void ShowVec(const std::string& name, Vector2 _value)
		{
			m_showData[name] = GenericData(GenericDataType::VEC2, _value);
		}
		void ShowVec(const std::string& name, Vector3 _value)
		{
			m_showData[name] = GenericData(GenericDataType::VEC3, _value);
		}
		void ShowVec(const std::string& name, Vector4 _value)
		{
			m_showData[name] = GenericData(GenericDataType::VEC4, _value);
		}
		void ShowColor(const std::string& name, Color3F _value)
		{
			m_showData[name] = GenericData(GenericDataType::COLOR3, _value);
		}
		void ShowColor(const std::string& name, Color4F _value)
		{
			m_showData[name] = GenericData(GenericDataType::COLOR4, _value);
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
