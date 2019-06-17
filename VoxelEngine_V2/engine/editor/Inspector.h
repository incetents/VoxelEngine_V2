// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../math/Color.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	static class Inspector : public Singleton<class Inspector>
	{
#ifdef GLOBAL_IMGUI
	private:
		Color3F LabelColorClipboard;
	public:
		void Draw();
#else
	public:
		void Draw() {}
#endif

	} SingletonInstance(Inspector);
}