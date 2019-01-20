// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "window.h"

#include "glfwCallbacks.h"
#include "../opengl/glUtil.h"

#include "../utilities/logger.h"

#include <GLFW/glfw3.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

namespace Vxl
{
	bool Window::InitGLFW()
	{
		return glfwInit();
	}
	void Window::Setup(
		const std::string& name,
		UINT width,
		UINT height)
	{
		if (m_setup)
			return;

		m_name = name;
		m_size[0] = width;
		m_size[1] = height;

		m_window = glfwCreateWindow(m_size[0], m_size[1], m_name.c_str(), NULL, NULL);
		if (!m_window)
		{
			Logger.error("Glfw could not create a new window: " + m_name);
			return;
		}

		glfwMakeContextCurrent(m_window);

		// Setup callbacks
		glfwSetKeyCallback				(m_window, glfwCallbacks::Keyboard);
		glfwSetCursorPosCallback		(m_window, glfwCallbacks::Mouse_Position);
		glfwSetMouseButtonCallback		(m_window, glfwCallbacks::Mouse_Button);
		glfwSetScrollCallback			(m_window, glfwCallbacks::Mouse_Scroll);
		glfwSetCursorEnterCallback		(m_window, glfwCallbacks::Mouse_Focus);
		glfwSetWindowFocusCallback		(m_window, glfwCallbacks::Window_Focus);
		glfwSetWindowCloseCallback		(m_window, glfwCallbacks::Exit_Button);
		glfwSetWindowSizeCallback		(m_window, glfwCallbacks::Window_Resize);
		glfwSetFramebufferSizeCallback	(m_window, glfwCallbacks::Resolution_Resize);

		// IMGUI Setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui_ImplGlfw_InitForOpenGL(GetContext(), true);
		ImGui_ImplOpenGL3_Init("#version 430");

		ImGui::StyleColorsDark();

		// Glew/Opengl Init
		glUtil::initGlew();
		glUtil::initHints();

		std::cout << "OpenGL Vers. " << glUtil::getOpenGLVersion() << std::endl;
		std::cout << glUtil::getRendererVersion() << std::endl;
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

		m_setup = true;
	}
	void Window::Reload()
	{
		Destroy();
		m_setup = false;
		Setup(m_name, m_size[0], m_size[1]);
	}
	void Window::Destroy()
	{
		glfwDestroyWindow(m_window);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void Window::StartFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	void Window::EndFrame()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		Update();
	}

	void Window::SetPosition(UINT x, UINT y)
	{
		glfwSetWindowPos(m_window, x, y);
	}
	void Window::SetSize(UINT width, UINT height)
	{
		glfwSetWindowSize(m_window, width, height);
	}
	void Window::SetSizeLimits(UINT xmin, UINT xmax, UINT ymin, UINT ymax)
	{
		glfwSetWindowSizeLimits(m_window, xmin, ymin, xmax, ymax);
	}
	void Window::SetVSynch(bool state)
	{
		glfwSwapInterval((int)state);
	}

	void Window::GetPosition(int& x, int& y)
	{
		glfwGetWindowPos(m_window, &x, &y);
	}
	void Window::GetSize(int& width, int& height)
	{
		glfwGetWindowSize(m_window, &width, &height);
	}

	void Window::Update()
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	void Window::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_window);
	}
}