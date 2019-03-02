// Copyright (c) 2019 Emmanuel Lajeunesse
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
