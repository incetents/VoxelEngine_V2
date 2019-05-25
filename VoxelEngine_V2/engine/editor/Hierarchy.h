// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class Entity;

	static class Hierarchy : public Singleton<class Hierarchy>
	{
		friend class RenderManager;
		friend class Inspector;
	private:
		// Utility
		void DisplayEntity(Entity* _ent, int _depth);
	public:

		// Behaviour
		void Draw();

	} SingletonInstance(Hierarchy);
}