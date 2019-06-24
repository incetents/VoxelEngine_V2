// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Input.h"

#include "../window/window.h"

namespace Vxl
{
	void InputMutator::setMouse(int xpos, int ypos)
	{
		Input.m_MousePos[0] = xpos;
		Input.m_MousePos[1] = ypos;

		Input.UpdateDragDeltaPos();

		VXL_ASSERT(Window.GetViewportWidth() > 0, "Window Viewport Width is 0");
		VXL_ASSERT(Window.GetViewportHeight() > 0, "Window Viewport Height is 0");

		Input.m_MousePosViewport[0] = (float)(xpos - Window.GetViewportOffsetX()) / (float)Window.GetViewportWidth();
		Input.m_MousePosViewport[1] = (float)(ypos - Window.GetViewportOffsetY()) / (float)Window.GetViewportHeight();
	}
}