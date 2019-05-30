// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/GlobalMacros.h"
#include "../utilities/singleton.h"

namespace Vxl
{
	static class ShaderErrors : public Singleton<class ShaderErrors>
	{
	public:

#ifdef GLOBAL_IMGUI
		void Draw();
#else
		void Draw() {}
#endif

	} SingletonInstance(ShaderErrors);
}
