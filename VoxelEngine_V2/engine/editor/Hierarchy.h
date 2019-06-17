// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	class Entity;

	static class Hierarchy : public Singleton<class Hierarchy>
	{
#ifdef GLOBAL_IMGUI
		friend class RenderManager;
		friend class Inspector;
	private:
		// Utility
		void DisplayEntity(Entity* _ent, int _depth);
	public:

		// Behaviour
		void Draw();
#else
	public:
		void Draw() {}
#endif

	} SingletonInstance(Hierarchy);
}