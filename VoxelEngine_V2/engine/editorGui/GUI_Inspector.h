// Copyright (c) 2020 Emmanuel Lajeunesse
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
		// Settings
		float DragSpeed = 0.1f;
		const float Empty3[3] = { 0, 0, 0 };
		float ImageSize = 50.0f;

		void DrawTransform();
		void DrawMaterial();
	public:
		void Draw() override;
#else
	public:
		void Draw() override {}
#endif

	} SingletonInstance(Inspector);
}