// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include <Windows.h>
#include <string>

struct GLFWwindow;

namespace Vxl
{
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
		UINT		 m_size[2]; // Window Size that can change
		UINT		 m_resolution[2]; // Window Resolution (fbos and cameras will use this)
		float		 m_aspectRatio = 1.0f; // Based on Window size (resolution shouldn't affect this)
		bool		 m_useCustomAspectRatio = false;
		float		 m_customAspectRatio = 1.0f;

		void UpdateSizes(UINT width, UINT height);
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
		inline UINT GetWidth()	 const
		{
			return m_size[0];
		}
		inline UINT GetHeight()  const
		{
			return m_size[1];
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

		void GetPosition(int& x, int& y);
		void GetSize(int& width, int& height);

		float GetAspectRatio(void) const
		{
			if (m_useCustomAspectRatio)
				return m_customAspectRatio;
			else
				return m_aspectRatio;
		}

		UINT GetSizeWidth(void) const
		{
			return m_size[0];
		}
		UINT GetSizeHeight(void) const
		{
			return m_size[1];
		}

		UINT GetResolutionWidth(void) const
		{
			return m_resolution[0];
		}
		UINT GetResolutionHeight(void) const
		{
			return m_resolution[1];
		}

		void ViewportToWindowSize();
		void ViewportToWindowResolution();
		
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

