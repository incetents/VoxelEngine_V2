// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

#define SHADER_LINE_MAX 100000

namespace Vxl
{
	static class ShaderCodeViewer : public Singleton<class ShaderCodeViewer>, public GuiWindow
	{
		std::string m_linesCounted;
	public:
		ShaderCodeViewer();

#ifdef GLOBAL_IMGUI
		void Draw() override;
#else
		void Draw() override {}
#endif

	} SingletonInstance(ShaderCodeViewer);
}
