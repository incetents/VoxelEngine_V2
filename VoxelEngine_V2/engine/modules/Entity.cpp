// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../opengl/glUtil.h"
#include "../opengl/Mesh.h"
#include "../modules/Material.h"
#include "../modules/RenderManager.h"
#include "../utilities/Time.h"

namespace Vxl
{
	DatabaseSet<Entity> Entity::m_database;

	Entity::Entity()
	{
		AddComponent(&m_transform, this);
		AddComponent(&m_meshRenderer, this);
		AddComponent(&m_material, this);
	}

	Entity* Entity::Create()
	{
		// Default Name
		Entity* E = new Entity();
		m_database.Set(E);
		return E;
	}

	Entity::~Entity()
	{
		if (m_material.GetBase())
			RenderManager.RemoveEntity(this);
	}

	void Entity::SetMaterial(Material* _base)
	{
		// Do nothing if no change
		if (_base == m_material.GetBase())
			return;

		// Remove material from existing render section
		// Doesn't get called if it originally had no material base
		if(m_material.GetBase())
			RenderManager.RemoveEntity(this);

		m_material.SetBase(_base);

		RenderManager.AddEntity(this);
	}
	Material* Entity::GetMaterial(void) const
	{
		return m_material.GetBase();
	}
	UINT Entity::GetMaterialOrder(void) const
	{
		return m_material.GetOrder();
	}

	void Entity::Update()
	{

	}

	void Entity::Draw()
	{
		m_material.Bind(!m_isColoredObject);

		m_mesh->Draw();
	}

}

