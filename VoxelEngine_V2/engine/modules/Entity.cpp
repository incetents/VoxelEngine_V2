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

#undef max
#undef min

namespace Vxl
{
	bool Entity::operator< (const Entity& other) const
	{
		Mesh* mesh1 = Assets.getMesh(getMesh());
		Mesh* mesh2 = Assets.getMesh(other.getMesh());
		if (!mesh1 || !mesh2)
			return false;

		return mesh1->getVAOID() < mesh2->getVAOID();
	}

	Entity::Entity(const std::string& name)
		: SceneNode(SceneNodeType::ENTITY, name)
	{
	}
	Entity::~Entity()
	{
	}

	// Mesh
	void Entity::setMesh(MeshIndex index)
	{
		m_mesh = index;
		m_transform.updateValues();
	}
	// Material
	void Entity::setMaterial(MaterialIndex index)
	{
		m_material = index;
		// Rendermanager must resort Entities
		RenderManager.dirtyEntitySequence();
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
			Quaternion _rotation = m_transform.getWorldRotation().Inverse();
			Vector3 _meshScale = _mesh->getScale();
			Vector3 _center = _rotation * (_mesh->getCenter() * m_transform.getWorldScale());

			Vector3 _right = m_transform.getRight() * 0.5f * _meshScale.x * m_transform.getWorldScale().x;
			Vector3 _up = m_transform.getUp() * 0.5f * _meshScale.y * m_transform.getWorldScale().y;
			Vector3 _forward = m_transform.getForward() * 0.5f * _meshScale.z * m_transform.getWorldScale().z;

			m_OBB[0] = _center - _right - _up - _forward;
			m_OBB[1] = _center + _right - _up - _forward;
			m_OBB[2] = _center - _right + _up - _forward;
			m_OBB[3] = _center + _right + _up - _forward;
			m_OBB[4] = _center - _right - _up + _forward;
			m_OBB[5] = _center + _right - _up + _forward;
			m_OBB[6] = _center - _right + _up + _forward;
			m_OBB[7] = _center + _right + _up + _forward;

			m_AABB[0] = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			m_AABB[1] = Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

			for (UINT i = 0; i < 8; i++)
			{
				m_AABB[0] = Vector3::Min(m_AABB[0], m_OBB[i]);
				m_AABB[1] = Vector3::Max(m_AABB[1], m_OBB[i]);
			}
		}
	}
}

