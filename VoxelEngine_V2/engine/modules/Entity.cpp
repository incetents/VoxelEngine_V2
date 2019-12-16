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
			Vector3 _meshScale = _mesh->getScale();
			Vector3 _center = Matrix3x3(m_transform.getModel()) * _mesh->getCenter();

			Vector3 _right = m_transform.getRight() * _meshScale.x * m_transform.getWorldScale().x;
			Vector3 _up = m_transform.getUp() * _meshScale.y * m_transform.getWorldScale().y;
			Vector3 _forward = m_transform.getForward() * _meshScale.z * m_transform.getWorldScale().z;

			col_OBB.position = _center + m_transform.getWorldPosition();
			col_OBB.right = _right;
			col_OBB.up = _up;
			col_OBB.forward = _forward;

			col_AABB = col_OBB.generateAABB();
		}
	}
}

