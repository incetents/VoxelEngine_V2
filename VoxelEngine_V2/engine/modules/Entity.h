// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../opengl/MeshRenderer.h"
#include "../modules/Material.h"

namespace Vxl
{
	class Mesh;
	class Material;
	class ShaderProgram;

	class Entity : public ComponentHandler
	{
	public:
		Entity();
		virtual ~Entity() {}

		// Utility
		virtual void Update();
		virtual void Draw();

		// Data
		Transform m_transform;
		MeshRenderer m_meshRenderer;
		Material m_material;
		
		// Mesh Rendering
		Mesh* m_mesh;

		void SetMaterial(ShaderProgram* _shader);
	};
}

