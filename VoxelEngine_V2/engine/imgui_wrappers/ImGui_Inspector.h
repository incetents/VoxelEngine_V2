// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	static class Imgui_Inspector : public Singleton<class Imgui_Inspector>
	{
	private:

	public:
		void Draw();

	} SingletonInstance(Imgui_Inspector);;
}