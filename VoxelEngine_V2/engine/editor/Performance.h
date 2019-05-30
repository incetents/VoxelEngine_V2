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
		GPUTimer* _selected = nullptr;
	public:
		// Draw
		void Draw();
#else
	public:
		void Draw() {}
#endif

	} SingletonInstance(Performance);
}