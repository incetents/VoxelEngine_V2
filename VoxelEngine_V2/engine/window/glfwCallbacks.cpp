// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "glfwCallbacks.h"

#include "window.h"
#include "../utilities/Logger.h"

#include <iostream>
#include <GLFW/glfw3.h>

namespace Vxl
{
	// Callback
	void glfwCallbacks::Error(int error, const char* description)
	{
		std::string Msg = "GLFW ERROR: " + std::string(description);
		Logger.error(Msg);
		std::system("PAUSE");
	}
	void glfwCallbacks::Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			InputMutator.setKey(key, true);
			break;
		case GLFW_RELEASE:
			InputMutator.setKey(key, false);
			break;
		}
	}
	void glfwCallbacks::Mouse_Position(GLFWwindow* window, double xpos, double ypos)
	{
		InputMutator.setMouse(static_cast<int>(xpos), static_cast<int>(ypos));
	}
	void glfwCallbacks::Mouse_Button(GLFWwindow* window, int button, int action, int mods)
	{
		InputMutator.setMouseButton(button, (action == GLFW_PRESS));
	}
	void glfwCallbacks::Mouse_Scroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		InputMutator.setScroll(xoffset, yoffset);
	}
	void glfwCallbacks::Mouse_Focus(GLFWwindow* window, int entered)
	{
		Window.m_mouseFocus = (entered > 0 ? true : false);
	}
	void glfwCallbacks::Window_Focus(GLFWwindow* window, int focused)
	{
		Window.m_windowFocus = (focused > 0 ? true : false);
	}
	void glfwCallbacks::Exit_Button(GLFWwindow* window)
	{
		Window.m_close = true;
	}
	void glfwCallbacks::Window_Resize(GLFWwindow* window, int width, int height)
	{
		Window.UpdateSizes((UINT)width, (UINT)height);
	}
	void glfwCallbacks::Window_Move(GLFWwindow* window, int xpos, int ypos)
	{
		Window.m_position[0] = (UINT)xpos;
		Window.m_position[1] = (UINT)ypos;
	}
	void glfwCallbacks::Drag_File(GLFWwindow* window, int count, const char** paths)
	{
		for (int i = 0; i < count; i++)
		{
			Window.m_addedPaths.push_back(std::string(paths[i]));
			std::cout << "Added FilePath: " << (paths[i]) << std::endl;
		}
	}
}