// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	static class Inspector : public Singleton<class Inspector>
	{
	private:

	public:
		void Draw();

	} SingletonInstance(Inspector);;
}