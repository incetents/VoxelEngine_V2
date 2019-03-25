// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class GPUTimer;

	static class Performance : public Singleton<class Performance>
	{
	private:
		GPUTimer* _selected = nullptr;
	public:
		// Draw
		void Draw();

	} SingletonInstance(Performance);
}