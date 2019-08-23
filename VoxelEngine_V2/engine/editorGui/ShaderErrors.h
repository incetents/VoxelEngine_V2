// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	static class ShaderErrors : public Singleton<class ShaderErrors>, public GuiWindow
	{
	public:

		bool HasErrors();

#ifdef GLOBAL_IMGUI
		void Draw() override;
#else
		void Draw() override {}
#endif

	} SingletonInstance(ShaderErrors);
}
