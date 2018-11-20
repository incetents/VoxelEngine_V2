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
		AddComponent(&m_transform);
		AddComponent(&m_meshRenderer);
	}

	void Entity::Update()
	{

	}

	void Entity::Draw()
	{
		if (m_mesh && m_material)
		{
			m_material->Bind();

			//if(_shader->check)
			//_shader->SetUniform("model", m_transform.getModel());

			m_mesh->Draw();
		}
	}

	void Entity::SetMaterial(Material* material)
	{
		m_material = material;
		if (material->m_hasModelMatrix)
		{
			material->m_modelMatrix = m_transform.getModel();
		}
	}
}

