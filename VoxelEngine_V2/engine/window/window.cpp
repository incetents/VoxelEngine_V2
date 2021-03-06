// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "window.h"

#include "glfwCallbacks.h"

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"
#include "../rendering/RenderManager.h"

#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include "../rendering/Graphics.h"

#ifdef GLOBAL_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/implement/imgui_impl_glfw.h"
#include "../imgui/implement/imgui_impl_opengl3.h"
#endif

namespace Vxl
{
	void Window::Setup(
		const std::string& name,
		uint32_t width,
		uint32_t height)
	{
		if (m_setup)
			return;

		if (!glfwInit())
			return;

		m_name = name;
		UpdateSizes(width, height);
		m_size[0] = width;
		m_size[1] = height;

		// Debug Context
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		// Set OpenGL Version for Context
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
		// Set OpenGL to Core Mode
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_size[0], m_size[1], m_name.c_str(), NULL, NULL);
		if (!m_window)
		{
			Logger.error("Glfw could not create a new window: " + m_name);
			return;
		}
		glfwMakeContextCurrent(m_window);

		// default position
		SetPosition(393, 296);
		// default cursor
		SetCursor(CursorMode::NORMAL);

		// Initial Window Position
		int posx, posy;
		glfwGetWindowPos(m_window, &posx, &posy);
		m_position[0] = (uint32_t)posx;
		m_position[1] = (uint32_t)posy;

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
		
		// Graphics Setup
		if(!Graphics::Setup())
			VXL_ASSERT(false, "Glew failed to initialized");
		Graphics::initHints();

		// Get version info
		Logger.log("~~~~~~~~~~~~~~~~~~");
		Logger.log("Renderer: " + Graphics::Gpu_Renderer);
		Logger.log("OpenGL version supported: " + Graphics::Gpu_OpenGLVersion);
		Logger.log("Vendor: " + Graphics::Gpu_Vendor);
		Logger.log("~~~~~~~~~~~~~~~~~~");

		

#ifdef GLOBAL_IMGUI
		// IMGUI Setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		// Settings
		ImGui::StyleColorsDark();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingWithShift = false;
		static bool once = true;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplGlfw_InitForOpenGL(GetContext(), true);
		ImGui_ImplOpenGL3_Init(Graphics::GLSL_Version.c_str());
#endif
		// Create first frame empty
		// (so it can create all gl assets necesary here without interfering with opengl error callback)
		StartFrame();

#ifdef GLOBAL_IMGUI
		ImGui::NewFrame();
		ImGui::Render();
#endif

		EndFrame();

		// opengl error callback
		Graphics::InitOpenGLDebugCallback();

		m_setup = true;
	}
	void Window::Reload()
	{
		Shutdown();
		m_setup = false;
		Setup(m_name, m_size[0], m_size[1]);
	}
	void Window::UpdateSizes(uint32_t width, uint32_t height)
	{
		m_size[0] = width;
		m_size[1] = height;
		m_aspectRatio = (float)width / (float)height;
		// Update aspect ratios for all related cameras
		RenderManager.UpdateAllWindowAspectCameras();
		// Update screen sizes for viewport rendering
		UpdateViewport();
		// Update all FBOs in fullscreen
		RenderManager.ReloadViewportFBOS();
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
			uint32_t customWidth = (uint32_t)((float)m_size[1] * m_customAspectRatio);
			uint32_t customHeight = (uint32_t)((float)m_size[0] / m_customAspectRatio);

			// Width clamp
			if (customWidth < m_size[0])
			{
				uint32_t offsetWidth = (m_size[0] >> 1) - (customWidth >> 1);
				m_viewportOffset[0] = offsetWidth;
				m_viewportOffset[1] = 0;
				m_viewportSize[0] = customWidth;
				m_viewportSize[1] = m_size[1];
			}
			// Height clamp
			else
			{
				
				uint32_t offsetHeight = (m_size[1] >> 1) - (customHeight >> 1);
				m_viewportOffset[0] = 0;
				m_viewportOffset[1] = offsetHeight;
				m_viewportSize[0] = m_size[0];
				m_viewportSize[1] = customHeight;
			}
		}
	}

	void Window::StartFrame()
	{
#ifdef GLOBAL_IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
#endif
	}
	void Window::EndFrame()
	{
#ifdef GLOBAL_IMGUI
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		auto& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			//GLFWwindow* backup_current_context = m_window;
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(m_window);
		}
#endif

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	void Window::SetPosition(uint32_t x, uint32_t y)
	{
		glfwSetWindowPos(m_window, x, y);
	}
	void Window::SetSize(uint32_t width, uint32_t height)
	{
		UpdateSizes(width, height);
		glfwSetWindowSize(m_window, width, height);
	}
	void Window::SetSizeLimits(uint32_t xmin, uint32_t xmax, uint32_t ymin, uint32_t ymax)
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
		// Update aspect ratios for all related cameras
		RenderManager.UpdateAllWindowAspectCameras();
		// Update viewport sizes
		UpdateViewport();
		// Update Viewport FBOs
		RenderManager.ReloadViewportFBOS();
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

	void Window::SetIcon(const std::string& filePath)
	{
		GLFWimage icons[1];
		icons[0].pixels = SOIL_load_image(filePath.c_str(), &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
		VXL_RETURN_ON_FAIL(icons[0].pixels != nullptr, "Failed to load icon for window");
		
		glfwSetWindowIcon(m_window, 1, icons);
		SOIL_free_image_data(icons[0].pixels);
	}

	void Window::SetMaximized(bool state)
	{
		if (state)
			glfwMaximizeWindow(m_window);
		else
			glfwRestoreWindow(m_window);
	}

	bool Window::IsCursorOnImguiWindow()
	{
#ifdef GLOBAL_IMGUI
		return ImGui::GetIO().WantCaptureMouse;
#else
		return false;
#endif
	}

	void Window::BindWindowViewport()
	{
		// Locked Aspect Ratio (Black bars on edges)
		if (m_useCustomAspectRatio)
			Graphics::SetViewport(m_viewportOffset[0], m_viewportOffset[1], m_viewportSize[0], m_viewportSize[1]);
		else
			Graphics::SetViewport(0, 0, m_size[0], m_size[1]);
	}

	void Window::Shutdown()
	{
#ifdef GLOBAL_IMGUI
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif
		glfwDestroyWindow(m_window);
		glfwTerminate();
		m_window = nullptr;
	}
}