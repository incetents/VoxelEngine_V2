// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../opengl/glUtil.h"
#include "../opengl/Mesh.h"
#include "../modules/Material.h"

namespace Vxl
{
	Entity::Entity()
	{
		AddComponent(&m_transform, this);
		AddComponent(&m_meshRenderer, this);
		AddComponent(&m_material, this);
	}

	void Entity::Update()
	{

	}

	void Entity::Draw()
	{
		m_material.Bind();

		m_mesh->Draw();
	}

	void Entity::SetMaterial(ShaderProgram* _shader)
	{
		m_material.SetShader(_shader);
		m_material.SetTransformReference(&m_transform);
	}
}

