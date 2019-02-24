// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	static class Imgui_ShaderErrors : public Singleton<class Imgui_ShaderErrors>
	{
	public:

		void Draw();

	} SingletonInstance(Imgui_ShaderErrors);
}
