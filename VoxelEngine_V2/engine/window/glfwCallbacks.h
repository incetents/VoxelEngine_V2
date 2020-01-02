// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../input/Input.h"

struct GLFWwindow;

namespace Vxl
{
	class glfwCallbacks
	{
		friend class InputMutator;

	public:
		// Error
		static void Error(int error, const char* description);
		// Input - Keyboard
		static void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
		// Input - Mouse
		static void Mouse_Position(GLFWwindow* window, double xpos, double ypos);
		// Input - Mouse Button
		static void Mouse_Button(GLFWwindow* window, int button, int action, int mods);
		// Input - Mouse Scroll
		static void Mouse_Scroll(GLFWwindow* window, double xoffset, double yoffset);
		// Input - Focus Window from Mouse
		static void Mouse_Focus(GLFWwindow* window, int entered);
		// Input - Focus Window
		static void Window_Focus(GLFWwindow* window, int focused);
		// Input - X Button
		static void Exit_Button(GLFWwindow* window);
		// Input - Resize Window
		static void Window_Resize(GLFWwindow* window, int width, int height);
		// Input - Move Window
		static void Window_Move(GLFWwindow* window, int xpos, int ypos);
		// Input - Drag File into Context
		static void Drag_File(GLFWwindow* window, int count, const char** paths);
	};
}

