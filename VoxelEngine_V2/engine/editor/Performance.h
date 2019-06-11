// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/GlobalMacros.h"

namespace Vxl
{
	class GPUTimer;

	static class Performance : public Singleton<class Performance>
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
		void Draw();
#else
	public:
		void Draw() {}
#endif

	} SingletonInstance(Performance);
}