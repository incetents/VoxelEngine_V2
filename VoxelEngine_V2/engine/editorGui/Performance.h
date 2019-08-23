// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	class GPUTimer;

	static class Performance : public Singleton<class Performance>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
	private:
		enum Mode
		{
			GPU,
			CPU
		};
		Mode m_mode = Mode::GPU;

		GPUTimer* m_GPUTimerselected = nullptr;
		CPUTimer* m_CPUTimerselected = nullptr;
	public:
		// Draw
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(Performance);
}