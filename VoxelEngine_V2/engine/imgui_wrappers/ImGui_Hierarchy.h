// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class Entity;

	static class Imgui_Hierarchy : public Singleton<class Imgui_Hierarchy>
	{
		friend class Imgui_Inspector;
	private:
		void DisplayEntity(Entity* _ent, int _depth);
	public:
		Entity* _selectedEntity = nullptr;
		void Draw();

	} SingletonInstance(Imgui_Hierarchy);;
}