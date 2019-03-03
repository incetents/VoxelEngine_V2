// Copyright (c) 2019 Emmanuel Lajeunesse
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

	void Entity::TransformChanged()
	{
		// Update Bounding Box information
		if (m_mesh != nullptr)
		{
			Vector3 VMin = m_mesh->GetVertexMin();
			Vector3 VMax = m_mesh->GetVertexMax();
			Quaternion Rotation = m_transform.getRotation();

			m_OBB[0] = Rotation * VMin; // x0 y0 z0
			m_OBB[1] = Rotation * Vector3(VMax.x, VMin.y, VMin.z); // x1 y0 z0
			m_OBB[2] = Rotation * Vector3(VMin.x, VMax.y, VMin.z); // x0 y1 z0
			m_OBB[3] = Rotation * Vector3(VMax.x, VMax.y, VMin.z); // x1 y1 z0
			m_OBB[4] = Rotation * Vector3(VMin.x, VMin.y, VMax.z); // x0 y0 z1
			m_OBB[5] = Rotation * Vector3(VMax.x, VMin.y, VMax.z); // x1 y0 z1
			m_OBB[6] = Rotation * Vector3(VMin.x, VMax.y, VMax.z); // x0 y1 z1
			m_OBB[7] = Rotation * VMax; // x1 y1 z1

			m_AABB[0] = Vector3::ZERO;
			m_AABB[1] = Vector3::ZERO;
			for (UINT i = 0; i < 8; i++)
			{
				m_AABB[0] = Vector3::Min(m_AABB[0], m_OBB[i]);
				m_AABB[1] = Vector3::Max(m_AABB[1], m_OBB[i]);
			}
		}
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

