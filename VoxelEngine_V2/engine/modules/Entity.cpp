// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../opengl/glUtil.h"
#include "../opengl/Mesh.h"
#include "../modules/Material.h"
#include "../modules/RenderManager.h"
#include "../utilities/Time.h"

#include <limits>

namespace Vxl
{
	DatabaseSet<Entity> Entity::m_database;

	Entity::Entity(const std::string& displayName)
	{
		m_name = displayName;

		AddComponent(&m_transform, this);
		AddComponent(&m_material, this);
	}

	Entity* Entity::Create(const std::string& displayName)
	{
		// Default Name
		Entity* E = new Entity(displayName);
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
			// Update OBB 
			Vector4 VMin = Vector4(m_mesh->GetVertexMin(), 1);
			Vector4 VMax = Vector4(m_mesh->GetVertexMax(), 1);
			Matrix4x4 Model = m_transform.getModel();
			Model.TransposeSelf();

			m_OBB[0] = (Model * VMin); // x0 y0 z0
			m_OBB[1] = (Model * Vector4(VMax.x, VMin.y, VMin.z, 1)); // x1 y0 z0
			m_OBB[2] = (Model * Vector4(VMin.x, VMax.y, VMin.z, 1)); // x0 y1 z0
			m_OBB[3] = (Model * Vector4(VMax.x, VMax.y, VMin.z, 1)); // x1 y1 z0
			m_OBB[4] = (Model * Vector4(VMin.x, VMin.y, VMax.z, 1)); // x0 y0 z1
			m_OBB[5] = (Model * Vector4(VMax.x, VMin.y, VMax.z, 1)); // x1 y0 z1
			m_OBB[6] = (Model * Vector4(VMin.x, VMax.y, VMax.z, 1)); // x0 y1 z1
			m_OBB[7] = (Model * VMax); // x1 y1 z1

#pragma push_macro("MACRONAME")
#undef max
#undef min
			m_AABB[0] = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			m_AABB[1] = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
#pragma pop_macro("MACRONAME")

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

