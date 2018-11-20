// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../opengl/MeshRenderer.h"

namespace Vxl
{
	class Mesh;
	class Material;

	class Entity : public ComponentHandler
	{
	private:
		// Material
		Material* m_material;
		
	public:
		Entity();
		virtual ~Entity() {}

		// Utility
		virtual void Update();
		virtual void Draw();

		// Data
		Transform m_transform;
		MeshRenderer m_meshRenderer;
		
		// Mesh Rendering
		Mesh* m_mesh;

		void SetMaterial(Material*);
	};
}

