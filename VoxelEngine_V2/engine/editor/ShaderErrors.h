// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	static class ShaderErrors : public Singleton<class ShaderErrors>
	{
	public:

		void Draw();

	} SingletonInstance(ShaderErrors);
}
