// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include <Windows.h>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Vxl
{
	enum class CursorMode
	{
		NORMAL,
		HIDDEN,
		LOCKED
	};

	static class Window : public Singleton<class Window>
	{
		friend class glfwCallbacks;
	private:
		bool		 m_setup		= false;
		bool		 m_close		= false;
		bool		 m_mouseFocus	= false;
		bool		 m_windowFocus	= false;
		GLFWwindow*  m_window;
		std::string  m_name;
		UINT		 m_position[2]; // Window position
		UINT		 m_size[2]; // Window Size that can change
		UINT		 m_resolution[2]; // Window Resolution (fbos and cameras will use this)
		UINT		 m_viewportSize[2]; // Same as Window size except it can be smaller if custom aspect ratio makes the view smaller
		UINT		 m_viewportOffset[2];
		float		 m_aspectRatio = 1.0f; // Based on Window size (resolution shouldn't affect this)
		bool		 m_useCustomAspectRatio = false;
		float		 m_customAspectRatio = 1.0f;
		CursorMode   m_cursorMode = CursorMode::NORMAL;

		// FilePaths dragged into the Window context
		std::vector<std::string> m_addedPaths;

		void UpdateSizes(UINT width, UINT height);
		void UpdateViewport();

		// Update Cameras locked to the Windows Aspect Ratio
		void UpdateCameraAspectRatios();

		void Destroy();
		void Update();
	public:

		inline GLFWwindow* GetContext() const
		{
			return m_window;
		}
		inline bool	GetClosed()  const
		{
			return m_close;
		}
		inline bool	GetMouseFocused() const
		{
			return m_mouseFocus;
		}
		inline bool	GetWindowFocused() const
		{
			return m_windowFocus;
		}

		inline void Close()
		{
			m_close = true;
		}

		void SetPosition(UINT x, UINT y);
		void SetSize(UINT width, UINT height);
		void SetSizeLimits(UINT xmin, UINT xmax, UINT ymin, UINT ymax);
		void SetVSynch(bool state);
		void SetCustomAspectRatio(bool state, float aspect = 1.0f);
		void SetCursor(CursorMode mode);
		void SetCursorPosition(double x, double y);

		bool IsCursorOnImguiWindow();

		// Window Pos
		UINT GetPositionX(void) const
		{
			return m_position[0];
		}
		UINT GetPositionY(void) const
		{
			return m_position[1];
		}

		// Get Correct Aspect Ratio based on rendering
		float GetAspectRatio(void) const
		{
			if (m_useCustomAspectRatio)
				return m_customAspectRatio;
			else
				return m_aspectRatio;
		}

		// Returns Width of the entire window
		UINT GetWindowWidth(void) const
		{
			return m_size[0];
		}
		// Returns Height of the entire window
		UINT GetWindowHeight(void) const
		{
			return m_size[1];
		}
		// Returns Width of the rendered viewport
		UINT GetScreenWidth(void) const
		{
			return m_viewportSize[0];
		}
		// Returns X offset of the rendered viewport
		UINT GetScreenOffsetX(void) const
		{
			return m_viewportOffset[0];
		}
		// Returns Y offset of the rendered viewport
		UINT GetScreenOffsetY(void) const
		{
			return m_viewportOffset[1];
		}

		// Returns Height of the rendered viewport
		UINT GetScreenHeight(void) const
		{
			return m_viewportSize[1];
		}
		// Returns Width of backbuffer resolution
		UINT GetResolutionWidth(void) const
		{
			return m_resolution[0];
		}
		// Returns Height of backbuffer resolution
		UINT GetResolutionHeight(void) const
		{
			return m_resolution[1];
		}

		// Get Cursors active mode
		CursorMode GetCursor(void) const
		{
			return m_cursorMode;
		}

		void BindWindowViewport();
		
		bool InitGLFW();
		void Setup(
			const std::string& name,
			UINT width,
			UINT height);
		void Reload();

		void Shutdown();

		void StartFrame();
		void EndFrame();

	} SingletonInstance(Window);
}

