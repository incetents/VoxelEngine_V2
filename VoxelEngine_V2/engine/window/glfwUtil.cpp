// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "glfwUtil.h"

#include "../utilities/logger.h"

namespace Vxl
{
	bool glfwUtil::init()
	{
		if (!glfwInit())
		{
			Logger.error("GLFW Init Failed");
			return false;
		}

		return true;
	}
}