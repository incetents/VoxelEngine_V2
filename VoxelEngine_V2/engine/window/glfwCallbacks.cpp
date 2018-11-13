// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "glfwCallbacks.h"

#include "glfwUtil.h"
#include "window.h"

namespace Vxl
{
	// Callback
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
		Window.Resize((UINT)width, (UINT)height);
	}
	void glfwCallbacks::Resolution_Resize(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}