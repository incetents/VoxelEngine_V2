// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../math/Color.h"

namespace Vxl
{
	static class Inspector : public Singleton<class Inspector>
	{
	private:
		Color3F LabelColorClipboard;
	public:
		void Draw();

	} SingletonInstance(Inspector);;
}