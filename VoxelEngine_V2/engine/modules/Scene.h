// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Scene
	{
		friend class RenderManager;
	private:
		// Setup
		virtual void Setup() = 0;
		virtual void Destroy() = 0;
		inline void Reload()
		{
			Destroy();
			Setup();
		}
		
	public:
		// Behaviours
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void DrawImGui() {}

	};
}
