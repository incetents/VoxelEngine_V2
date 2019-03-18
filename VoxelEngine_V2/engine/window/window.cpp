// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "window.h"

#include "glfwCallbacks.h"
#include "../opengl/glUtil.h"

#include "../utilities/logger.h"
#include "../math/Camera.h"

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
		UpdateSizes(width, height);
		m_resolution[0] = width;
		m_resolution[1] = height;

		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_size[0], m_size[1], m_name.c_str(), NULL, NULL);
		if (!m_window)
		{
			Logger.error("Glfw could not create a new window: " + m_name);
			return;
		}
		// default position
		SetPosition(50, 50);
		// default cursor
		SetCursor(CursorMode::NORMAL);

		// Initial Window Position
		int posx, posy;
		glfwGetWindowPos(m_window, &posx, &posy);
		m_position[0] = (UINT)posx;
		m_position[1] = (UINT)posy;

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
		glfwSetWindowPosCallback		(m_window, glfwCallbacks::Window_Move);
		glfwSetDropCallback				(m_window, glfwCallbacks::Drag_File);
		glfwSetErrorCallback			(glfwCallbacks::Error);
		

		// IMGUI Setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForOpenGL(GetContext(), true);
		ImGui_ImplOpenGL3_Init("#version 430");

		ImGui::StyleColorsDark();

		// Glew/Opengl Init
		if (!glUtil::initGlew())
			assert(false);

		glUtil::initHints();
		glUtil::CheckVersion();

		// opengl error callback
		glUtil::initOpenglErrorCallback();

		m_setup = true;
	}
	void Window::Reload()
	{
		Destroy();
		m_setup = false;
		Setup(m_name, m_size[0], m_size[1]);
	}
	void Window::UpdateSizes(UINT width, UINT height)
	{
		m_size[0] = width;
		m_size[1] = height;
		m_aspectRatio = (float)width / (float)height;
		// If using normal aspect ratio, 
		UpdateCameraAspectRatios();
		// Update screen sizes for viewport rendering
		UpdateViewport();
	}
	void Window::UpdateViewport()
	{
		// If using normal aspect ratio, 
		if (!m_useCustomAspectRatio)
		{
			// update viewport info to be the same as window data
			m_viewportOffset[0] = 0;
			m_viewportOffset[1] = 0;
			m_viewportSize[0] = m_size[0];
			m_viewportSize[1] = m_size[1];
		}
		// Locked Aspect Ratio (Black bars on edges)
		else
		{
			// Width clamp
			if (m_size[0] > m_size[1])
			{
				UINT customWidth = (UINT)((float)m_size[1] * m_customAspectRatio);
				UINT offsetWidth = (m_size[0] >> 1) - (customWidth >> 1);
				m_viewportOffset[0] = offsetWidth;
				m_viewportOffset[1] = 0;
				m_viewportSize[0] = customWidth;
				m_viewportSize[1] = m_size[1];
			}
			// Height clamp
			else
			{
				UINT customHeight = (UINT)((float)m_size[0] / m_customAspectRatio);
				UINT offsetHeight = (m_size[1] >> 1) - (customHeight >> 1);
				m_viewportOffset[0] = 0;
				m_viewportOffset[1] = offsetHeight;
				m_viewportSize[0] = m_size[0];
				m_viewportSize[1] = customHeight;
			}
		}
	}
	void Window::UpdateCameraAspectRatios()
	{
		// Update aspect ratio for all existing cameras that are locked to this aspect ratio
		auto Cameras = Camera::m_database.Get();
		for (auto it = Cameras.begin(); it != Cameras.end(); it++)
		{
			auto Cam = it->second;
			if (Cam->isPerspective() && Cam->m_projection->IsWindowAspectRatio())
			{
				Cam->m_projection->Update_FovAspect(Cam->m_projection->getFov(), GetAspectRatio());
			}
		}
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
		UpdateSizes(width, height);
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
	void Window::SetCustomAspectRatio(bool state, float aspect)
	{
		// Flags
		m_useCustomAspectRatio = state;
		m_customAspectRatio = aspect;
		// Update Aspect ratios
		UpdateCameraAspectRatios();
		// Update viewport sizes
		UpdateViewport();
	}
	void Window::SetCursor(CursorMode mode)
	{
		m_cursorMode = mode;
		switch (mode)
		{
		case CursorMode::NORMAL:
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case CursorMode::HIDDEN:
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
		case CursorMode::LOCKED :
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}
	}
	void Window::SetCursorPosition(double x, double y)
	{
		glfwSetCursorPos(m_window, x, y);
	}

	bool Window::IsCursorOnImguiWindow()
	{
		return ImGui::GetIO().WantCaptureMouse;
	}

	void Window::BindWindowViewport()
	{
		// Locked Aspect Ratio (Black bars on edges)
		if (m_useCustomAspectRatio)
			glUtil::viewport(m_viewportOffset[0], m_viewportOffset[1], m_viewportSize[0], m_viewportSize[1]);
		else
			glUtil::viewport(0, 0, m_size[0], m_size[1]);
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