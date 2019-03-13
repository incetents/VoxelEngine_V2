// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class Entity;

	static class Hierarchy : public Singleton<class Hierarchy>
	{
		friend class Inspector;
	private:
		void DisplayEntity(Entity* _ent, int _depth);
	public:
		Entity* _selectedEntity = nullptr;
		void Draw();

	} SingletonInstance(Hierarchy);;
}