// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "GuiWindow.h"

#include "../utilities/singleton.h"
#include "../math/Color.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	static class Inspector : public Singleton<class Inspector>, public GuiWindow
	{
#ifdef GLOBAL_IMGUI
	private:
		Color3F LabelColorClipboard;
	public:
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(Inspector);
}