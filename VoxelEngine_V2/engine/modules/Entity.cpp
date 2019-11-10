// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/Camera.h"

#include "../rendering/Mesh.h"
#include "../rendering/RenderManager.h"

#include "../utilities/Time.h"
#include "../utilities/Logger.h"
#include "../utilities/Asset.h"

#include <algorithm>
#include <limits>

namespace Vxl
{
	// Unique IDs
	Entity::Entity(const std::string& name)
		: m_name(name)
	{
		addComponent(&m_transform);
	}
	Entity::~Entity()
	{
	}

	// Mesh
	void Entity::setMesh(MeshIndex index)
	{
		m_mesh = index;
	}
	// Material
	void Entity::setMaterial(MaterialIndex index)
	{
		m_material = index;
	}

	bool Entity::IsFamilyActive()
	{
		// if no parent, check normal active state
		Transform* parent = m_transform.getParent();
		if (parent == nullptr)
			return m_isActive;

		// auto fail if off
		if (!m_isActive)
			return false;

		// iterate through all parents
		while (parent != nullptr)
		{
			Entity* owner = Assets::getEntity(parent->m_owner);
			VXL_ASSERT(owner, "Component not attached to entity");
			if (!owner->m_isActive)
				return false;
			// Acquire new parent
			else
				parent = owner->m_transform.getParent();
		}
		// If no parents failed, that means it's all good
		return true;
	}

	// Update Bounding Box from Mesh
	void Entity::UpdateBoundingBoxCheap()
	{
		Mesh* _mesh = SceneAssets.getMesh(m_mesh);
		VXL_ASSERT(_mesh, "Cannot update bounding box without mesh");

		// Update Bounding Box information
		if (_mesh != nullptr)
		{
			// Update OBB
			Vector3 _scale = m_transform.getWorldScale() * (_mesh->GetVertexMax() - _mesh->GetVertexMin());
			Vector3 _right = m_transform.getRight() * _scale.x * 0.5f;
			Vector3 _up = m_transform.getUp() * _scale.y * 0.5f;
			Vector3 _forward = m_transform.getForward() * _scale.z * 0.5f;

			m_OBB[0] = - _right - _up - _forward; // x0 y0 z0
			m_OBB[1] =   _right - _up - _forward; // x1 y0 z0
			m_OBB[2] = - _right + _up - _forward; // x0 y1 z0
			m_OBB[3] =   _right + _up - _forward; // x1 y1 z0
			m_OBB[4] = - _right - _up + _forward; // x0 y0 z1
			m_OBB[5] =   _right - _up + _forward; // x1 y0 z1
			m_OBB[6] = - _right + _up + _forward; // x0 y1 z1
			m_OBB[7] =   _right + _up + _forward; // x1 y1 z1

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
}

