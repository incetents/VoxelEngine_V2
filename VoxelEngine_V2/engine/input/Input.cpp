// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Input.h"

#include "../math/Vector.h"

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

		Input.m_MousePosScreenspace[0] = Input.m_MousePosViewport[0] * 2.0f - 1.0f;
		Input.m_MousePosScreenspace[1] = Input.m_MousePosViewport[1] * 2.0f - 1.0f;
	}

	Vector2 Input::getMousePos() const
	{
		return Vector2(
			(float)m_MousePos[0],
			(float)m_MousePos[1]
		);
	}
	Vector2 Input::getMousePosViewport(bool flipY) const
	{
		return Vector2(
			(float)m_MousePosViewport[0],
			flipY ? 1.0f - (float)m_MousePosViewport[1] : (float)m_MousePosViewport[1]
		);
	}
	Vector2 Input::getMousePosScreenspace(bool flipY) const
	{
		return Vector2(
			(float)m_MousePosScreenspace[0],
			flipY ? -(float)m_MousePosScreenspace[1] : (float)m_MousePosScreenspace[1]
		);
	}
}