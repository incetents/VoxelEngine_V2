// Copyright(c) 2020 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

#include <Windows.h>

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#endif

namespace Vxl
{
	template<typename Type>
	class _Vector2;
	template<typename Type>
	class _Vector3;

	enum class KeyCode
	{
		SPACE = 32,
		APOSTROPHE = 39,
		COMMA = 44,
		MINUS = 45,
		PERIOD = 46,
		SLASH = 47,
		Num_0 = 48,
		Num_1 = 49,
		Num_2 = 50,
		Num_3 = 51,
		Num_4 = 52,
		Num_5 = 53,
		Num_6 = 54,
		Num_7 = 55,
		Num_8 = 56,
		Num_9 = 57,
		SEMICOLON = 59,
		EQUAL = 61,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LEFT_BRACKET = 91,
		BACKSLASH = 92,
		RIGHT_BRACKET = 93,
		GRAVE_ACCENT = 96,
		WORLD_1 = 161,
		WORLD_2 = 162,
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		BACKSPACE = 259,
		INSERT = 260,
		DELETEKEY = 261,
		RIGHT = 262,
		LEFT = 263,
		DOWN = 264,
		UP = 265,
		PAGE_UP = 266,
		PAGE_DOWN = 267,
		HOME = 268,
		END = 269,
		CAPS_LOCK = 280,
		SCROLL_LOCK = 281,
		NU_LOCK = 282,
		PRINT_SCREEN = 283,
		PAUSE = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KP_0 = 320,
		KP_1 = 321,
		KP_2 = 322,
		KP_3 = 323,
		KP_4 = 324,
		KP_5 = 325,
		KP_6 = 326,
		KP_7 = 327,
		KP_8 = 328,
		KP_9 = 329,
		KP_DECIMAL = 330,
		KP_DIVIDE = 331,
		KP_MULTIPLY = 332,
		KP_SUBTRACT = 333,
		KP_ADD = 334,
		KP_ENTER = 335,
		KP_EQUAL = 336,
		LEFT_SHIFT = 340,
		LEFT_CONTROL = 341,
		LEFT_ALT = 342,
		LEFT_SUPER = 343,
		RIGHT_SHIFT = 344,
		RIGHT_CONTROL = 345,
		RIGHT_ALT = 346,
		RIGHT_SUPER = 347,
		MENU = 348,
		TOTAL_KEYS
	};

	enum class MouseButton
	{
		LEFT = 0,
		MIDDLE,
		RIGHT
	};

	#define TotalKeys (UINT)KeyCode::TOTAL_KEYS
	#define TotalMouseButtons 8

	static class Input : public Singleton<class Input>
	{
		friend class InputMutator;
	protected:
		// Keyboard Data
		bool	m_Key_Previous[TotalKeys];
		bool	m_Key_Current[TotalKeys];
		// Mouse Data
		int		m_MousePrevPos[2] = { 0 };
		int		m_MousePos[2] = { 0 };
		int		m_MouseDeltaPos[2] = { 0 }; // Change in position during a frame
		int		m_MouseDragDeltaPos[2] = { 0 }; // Change in position during a frame while dragging [left click drag]
		float	m_MousePosViewport[2] = { 0 }; // Mouse position across the render viewport [Range = 0 -> 1 ]
		float	m_MousePosScreenspace[2] = { 0 }; // Mouse position across the render viewport [Range = -1 -> 1 ]
		bool	m_MouseButtonsPrev[TotalMouseButtons] = { false };
		bool	m_MouseButtons[TotalMouseButtons] = { false };
		double  m_MouseScroll[2] = { 0.0 };
		int		m_MouseLastLeftClickPos[2] = { 0 }; // Last clicked position

		inline void UpdateDragDeltaPos()
		{
			m_MouseDragDeltaPos[0] = m_MousePos[0] - m_MouseLastLeftClickPos[0];
			m_MouseDragDeltaPos[1] = m_MousePos[1] - m_MouseLastLeftClickPos[1];
		}

	public:
		// Keyboard Data
		inline bool   getKey(KeyCode K)		const
		{
#ifdef GLOBAL_IMGUI
			return  m_Key_Current[(int)K] && !ImGui::GetIO().WantCaptureKeyboard;
#else
			return  m_Key_Current[(int)K];
#endif
		}
		inline bool   getKeyDown(KeyCode K) const
		{
#ifdef GLOBAL_IMGUI
			return  m_Key_Current[(int)K] && !m_Key_Previous[(int)K] && !ImGui::GetIO().WantCaptureKeyboard;
#else
			return  m_Key_Current[(int)K] && !m_Key_Previous[(int)K];
#endif
		}
		inline bool   getKeyUp(KeyCode K)	const
		{
#ifdef GLOBAL_IMGUI
			return !m_Key_Current[(int)K] && m_Key_Previous[(int)K] && !ImGui::GetIO().WantCaptureKeyboard;
#else
			return !m_Key_Current[(int)K] && m_Key_Previous[(int)K];
#endif
		}
		// Mouse Data
		inline int	  getMousePosY(void) const { return m_MousePos[1]; }
		inline int	  getMousePosX(void) const { return m_MousePos[0]; }
		inline float  getMousePosViewportX(void) const { return m_MousePosViewport[0]; }
		inline float  getMousePosViewportY(bool flip = true) const { return flip ? 1.0f-m_MousePosViewport[1] : m_MousePosViewport[1]; }
		inline float  getMousePosScreenspaceX(void) const { return m_MousePosScreenspace[0]; }
		inline float  getMousePosScreenspaceY(bool flip = true) const { return flip ? - m_MousePosScreenspace[1] : m_MousePosScreenspace[1]; }
		_Vector2<float> getMousePos() const;
		_Vector2<float> getMousePosViewport(bool flipY = true) const;
		_Vector2<float> getMousePosScreenspace(bool flipY = true) const;
		inline int	  getMouseDeltaX(void) const { return m_MouseDeltaPos[0]; }
		inline int	  getMouseDeltaY(void) const { return m_MouseDeltaPos[1]; }
		inline int	  getMouseDragDeltaX(void) const { return m_MouseDragDeltaPos[0]; }
		inline int	  getMouseDragDeltaY(void) const { return m_MouseDragDeltaPos[1]; }
		bool		  getMouseButton(MouseButton M) const
		{
#ifdef GLOBAL_IMGUI
			return m_MouseButtons[static_cast<int>(M)] && !ImGui::GetIO().WantCaptureMouse;
#else
			return m_MouseButtons[static_cast<int>(M)];
#endif
		}
		bool		  getMouseButtonDown(MouseButton M) const
		{
#ifdef GLOBAL_IMGUI
			return m_MouseButtons[static_cast<int>(M)] && !m_MouseButtonsPrev[static_cast<int>(M)] && !ImGui::GetIO().WantCaptureMouse;
#else
			return m_MouseButtons[static_cast<int>(M)] && !m_MouseButtonsPrev[static_cast<int>(M)];
#endif
		}
		bool		  getMouseButtonUp(MouseButton M) const
		{
#ifdef GLOBAL_IMGUI
			return !m_MouseButtons[static_cast<int>(M)] && m_MouseButtonsPrev[static_cast<int>(M)] && !ImGui::GetIO().WantCaptureMouse;
#else
			return !m_MouseButtons[static_cast<int>(M)] && m_MouseButtonsPrev[static_cast<int>(M)];
#endif
		}
		inline int	  getMouseLastLeftClickX(void) const { return m_MouseLastLeftClickPos[0]; }
		inline int	  getMouseLastLeftClickY(void) const { return m_MouseLastLeftClickPos[1]; }
		inline double getHorizontalScroll(void)			const { return m_MouseScroll[0]; }
		inline double getVerticalScroll(void)			const { return m_MouseScroll[1]; }

		// Update at end of every input frame
		inline void Update()
		{
			memcpy(m_Key_Previous, m_Key_Current, sizeof(bool) * TotalKeys);
			memcpy(m_MouseButtonsPrev, m_MouseButtons, sizeof(bool) * TotalMouseButtons);

			m_MouseDeltaPos[0] = m_MousePos[0] - m_MousePrevPos[0];
			m_MouseDeltaPos[1] = m_MousePos[1] - m_MousePrevPos[1];

			m_MousePrevPos[0] = m_MousePos[0];
			m_MousePrevPos[1] = m_MousePos[1];
		}

	} SingletonInstance(Input);

	// Mutator of Input Values
	static class InputMutator : public Singleton<class InputMutator>
	{
	public:
		inline void setKey(int key, bool state)
		{
			Input.m_Key_Current[key] = state;
		}
		inline void setKey(KeyCode key, bool state)
		{
			Input.m_Key_Current[(int)key] = state;
		}
		void setMouse(int xpos, int ypos);
		inline void setMouseButton(int index, bool state)
		{
			Input.m_MouseButtons[index] = state;

			// Track previous left click
			if (index == (int)MouseButton::LEFT && state)
			{
				Input.m_MouseLastLeftClickPos[0] = Input.m_MousePos[0];
				Input.m_MouseLastLeftClickPos[1] = Input.m_MousePos[1];

				Input.UpdateDragDeltaPos();
			}
		}
		inline void setScroll(double x, double y)
		{
			Input.m_MouseScroll[0] = x;
			Input.m_MouseScroll[1] = y;
		}

	} SingletonInstance(InputMutator);
}