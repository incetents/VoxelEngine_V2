// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"
#include "../utilities/Containers.h"

#include "../math/Vector.h"
#include "../math/Color.h"
#include "../math/MathCore.h"

#include <map>
#include <utility>

namespace Vxl
{
	class Scene;
	class Camera;

	static class DevConsole : public Singleton<class DevConsole>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
	private:
		// All data to show
		std::map<std::string, RawData> m_showData;
		
		// All data to edit
		struct EditData
		{
			RawData data;
			bool isRange; // X is value, Y and Z are min and max (assumes it's _min vec3 of some sort)
			bool isColor; // Vec3 -> Color3; Vec4 -> Color4
		};
		std::map<std::string, EditData> m_editData;

		// Menu Mode
		enum class MenuState
		{
			MASTER,
			EDIT_DATA,
			SHOW_DATA
		};
		MenuState m_State = MenuState::MASTER;
		// Draw Menu Section
		void Draw_Master(Scene* scene);
		void Draw_ShowValues();
		void Draw_EditValues();
	public:

		// custom data
		bool	GetBool(const std::string& name, bool _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::BOOL, _default), false, false };

			return *m_editData[name].data.getData<bool>();
		}
		int		GetInt(const std::string& name, int _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::INT, _default), false, false };

			return *m_editData[name].data.getData<int>();
		}
		float	GetFloat(const std::string& name, float _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT, _default), false, false };

			return *m_editData[name].data.getData<float>();
		}
		double	GetDouble(const std::string& name, double _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::DOUBLE, _default), false, false };

			return *m_editData[name].data.getData<double>();
		}
		Vector2 GetVector(const std::string& name, Vector2 _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC2, _default), false, false };

			return *m_editData[name].data.getData<Vector2>();
		}
		Vector2i GetVector(const std::string& name, Vector2i _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::INT_VEC2, _default), false, false };

			return *m_editData[name].data.getData<Vector2i>();
		}
		Vector2d GetVector(const std::string& name, Vector2d _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::DOUBLE_VEC2, _default), false, false };

			return *m_editData[name].data.getData<Vector2d>();
		}
		Vector3 GetVector(const std::string& name, Vector3 _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC3, _default), false, false };

			return *m_editData[name].data.getData<Vector3>();
		}
		Vector3i GetVector(const std::string& name, Vector3i _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::INT_VEC3, _default), false, false };

			return *m_editData[name].data.getData<Vector3i>();
		}
		Vector3d GetVector(const std::string& name, Vector3d _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::DOUBLE_VEC3, _default), false, false };

			return *m_editData[name].data.getData<Vector3d>();
		}
		Vector4 GetVector(const std::string& name, Vector4 _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC4, _default), false, false };

			return *m_editData[name].data.getData<Vector4>();
		}
		Vector4i GetVector(const std::string& name, Vector4i _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::INT_VEC4, _default), false, false };

			return *m_editData[name].data.getData<Vector4i>();
		}
		Vector4d GetVector(const std::string& name, Vector4d _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::DOUBLE_VEC4, _default), false, false };

			return *m_editData[name].data.getData<Vector4d>();
		}
		Color3F GetColor(const std::string& name, Color3F _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC3, _default), false, true };

			return *m_editData[name].data.getData<Color3F>();
		}
		Color4F GetColor(const std::string& name, Color4F _default)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC4, _default), false, true };

			return *m_editData[name].data.getData<Color4F>();
		}

		int		GetIntRange(const std::string& name, int _default, int _min, int _max)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::INT_VEC3, Vector3i(_default, _min, _max)), true, false };

			return m_editData[name].data.getData<Vector3i>()->x;
		}
		float	GetFloatRange(const std::string& name, float _default, float _min, float _max)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::FLOAT_VEC3, Vector3(_default, _min, _max)), true, false };

			return m_editData[name].data.getData<Vector3>()->x;
		}
		double	GetDoubleRange(const std::string& name, double _default, double _min, double _max)
		{
			if (m_editData.find(name) == m_editData.end())
				m_editData[name] = EditData{ RawData(UniformType::DOUBLE_VEC3, Vector3d(_default, _min, _max)), true, false };

			return m_editData[name].data.getData<Vector3d>()->x;
		}

		// display data
		void RemoveShowData(const std::string& name)
		{
			if (m_showData.find(name) != m_showData.end())
				m_showData.erase(name);
		}

		void ShowBool(const std::string& name, bool _value)
		{
			m_showData[name] = RawData(UniformType::BOOL, _value);
		}
		void ShowInt(const std::string& name, int _value)
		{
			m_showData[name] = RawData(UniformType::INT, _value);
		}
		void ShowFloat(const std::string& name, float _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT, _value);
		}
		void ShowDouble(const std::string& name, double _value)
		{
			m_showData[name] = RawData(UniformType::DOUBLE, _value);
		}
		void ShowVector(const std::string& name, const Vector2& _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT_VEC2, _value);
		}
		void ShowVector(const std::string& name, const Vector2i& _value)
		{
			m_showData[name] = RawData(UniformType::INT_VEC2, _value);
		}
		void ShowVector(const std::string& name, const Vector2d& _value)
		{
			m_showData[name] = RawData(UniformType::DOUBLE_VEC2, _value);
		}
		void ShowVector(const std::string& name, const Vector3& _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT_VEC3, _value);
		}
		void ShowVector(const std::string& name, const Vector3i& _value)
		{
			m_showData[name] = RawData(UniformType::INT_VEC3, _value);
		}
		void ShowVector(const std::string& name, const Vector3d& _value)
		{
			m_showData[name] = RawData(UniformType::DOUBLE_VEC3, _value);
		}
		void ShowVector(const std::string& name, const Vector4& _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT_VEC4, _value);
		}
		void ShowVector(const std::string& name, const Vector4i& _value)
		{
			m_showData[name] = RawData(UniformType::INT_VEC4, _value);
		}
		void ShowVector(const std::string& name, const Vector4d& _value)
		{
			m_showData[name] = RawData(UniformType::DOUBLE_VEC4, _value);
		}
		void ShowColor(const std::string& name, const Color3F& _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT_VEC3, _value);
		}
		void ShowColor(const std::string& name, const Color4F& _value)
		{
			m_showData[name] = RawData(UniformType::FLOAT_VEC4, _value);
		}

#define DEV_GET_BOOL(name, value) DevConsole.GetBool(name, value)
#define DEV_GET_INT(name, value) DevConsole.GetInt(name, value)
#define DEV_GET_FLOAT(name, value) DevConsole.GetFloat(name, value)
#define DEV_GET_DOUBLE(name, value) DevConsole.GetDouble(name, value)
#define DEV_GET_VECTOR(name, value) DevConsole.GetVector(name, value)
#define DEV_GET_COLOR(name, value) DevConsole.GetColor(name, value)

#define DEV_GET_INT_RANGE(name, value, _min, _max) DevConsole.GetIntRange(name, value, _min, _max)
#define DEV_GET_FLOAT_RANGE(name, value, _min, _max) DevConsole.GetFloatRange(name, value, _min, _max)
#define DEV_GET_DOUBLE_RANGE(name, value, _min, _max) DevConsole.GetDoubleRange(name, value, _min, _max)

#define DEV_SHOW_BOOL(name, value) DevConsole.ShowBool(name, value)
#define DEV_SHOW_INT(name, value) DevConsole.ShowInt(name, value)
#define DEV_SHOW_FLOAT(name, value) DevConsole.ShowFloat(name, value)
#define DEV_SHOW_DOUBLE(name, value) DevConsole.ShowDouble(name, value)
#define DEV_SHOW_VECTOR(name, value) DevConsole.ShowVector(name, value)
#define DEV_SHOW_COLOR(name, value) DevConsole.ShowColor(name, value)

		// Draw
		void Draw() override;
#else

#define DEV_GET_BOOL(name, value) value
#define DEV_GET_INT(name, value) value
#define DEV_GET_FLOAT(name, value) value
#define DEV_GET_DOUBLE(name, value) value
#define DEV_GET_VECTOR(name, value) value
#define DEV_GET_COLOR(name, value) value

#define DEV_GET_INT_RANGE(name, value, _min, _max) value
#define DEV_GET_FLOAT_RANGE(name, value, _min, _max) value
#define DEV_GET_DOUBLE_RANGE(name, value, _min, _max) value

#define DEV_SHOW_BOOL(name, value) __noop
#define DEV_SHOW_INT(name, value) __noop
#define DEV_SHOW_FLOAT(name, value) __noop
#define DEV_SHOW_DOUBLE(name, value) __noop
#define DEV_SHOW_VECTOR(name, value) __noop
#define DEV_SHOW_COLOR(name, value)	__noop

	public:
		void Draw() override {}
#endif

	} SingletonInstance(DevConsole);
}
