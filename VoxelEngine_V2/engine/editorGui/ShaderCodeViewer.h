// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	static class ShaderCodeViewer : public Singleton<class ShaderCodeViewer>, public GuiWindow
	{
	public:

#ifdef GLOBAL_IMGUI
		void Draw() override;
#else
		void Draw() override {}
#endif

	} SingletonInstance(ShaderCodeViewer);
}
