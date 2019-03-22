// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	static class Performance : public Singleton<class Performance>
	{
	private:

	public:
		// Draw
		void Draw();

	} SingletonInstance(Performance);
}