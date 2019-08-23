// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Scene
	{
		friend class RenderManager;
	private:
		// Setup
		inline void Reload()
		{
			Destroy();
			Setup();
		}
		
	public:
		virtual void Setup() = 0;
		virtual void Destroy() = 0;
		// Behaviours
		virtual void Update() = 0; // update per frame
		virtual void UpdateFixed() = 0; // update at framerate
		virtual void Draw() = 0;

	};
}
