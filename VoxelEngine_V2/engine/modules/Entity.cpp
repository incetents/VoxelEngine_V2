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

		// Update Bounding Box information (mesh might be destroyed, that's why there's a nullptr check)
		if (_mesh != nullptr)
		{
			col_OBB = OBB(
				m_transform.getModel(),
				m_transform.getRight(),
				m_transform.getUp(),
				m_transform.getForward(),
				_mesh->getVertexMin(),
				_mesh->getVertexMax()
			);
			//	Vector3 meshMin = _mesh->getVertexMin();
			//	Vector3 meshMax = _mesh->getVertexMax();
			//	
			//	obbFuzzy[0] = m_transform.getModel() * meshMin;
			//	obbFuzzy[1] = m_transform.getModel() * Vector3(meshMax.x, meshMin.y, meshMin.z);
			//	obbFuzzy[2] = m_transform.getModel() * Vector3(meshMin.x, meshMax.y, meshMin.z);
			//	obbFuzzy[3] = m_transform.getModel() * Vector3(meshMax.x, meshMax.y, meshMin.z);
			//	obbFuzzy[4] = m_transform.getModel() * Vector3(meshMin.x, meshMin.y, meshMax.z);
			//	obbFuzzy[5] = m_transform.getModel() * Vector3(meshMax.x, meshMin.y, meshMax.z);
			//	obbFuzzy[6] = m_transform.getModel() * Vector3(meshMin.x, meshMax.y, meshMax.z);
			//	obbFuzzy[7] = m_transform.getModel() * meshMax;
			//	
			//	// Project Obb Fuzzy on each axis to get largest/smallest
			//	Vector3 minFuzzy = Vector3::MAX;
			//	Vector3 maxFuzzy = Vector3::MIN;
			//	for (int i = 0; i < 8; i++)
			//	{
			//		float fuzzyRight	= obbFuzzy[i].ProjectLength(m_transform.getRight());
			//		float fuzzyUp		= obbFuzzy[i].ProjectLength(m_transform.getUp());
			//		float fuzzyForward	= obbFuzzy[i].ProjectLength(m_transform.getForward());
			//	
			//		minFuzzy.x = min(minFuzzy.x, fuzzyRight);
			//		maxFuzzy.x = max(maxFuzzy.x, fuzzyRight);
			//	
			//		minFuzzy.y = min(minFuzzy.y, fuzzyUp);
			//		maxFuzzy.y = max(maxFuzzy.y, fuzzyUp);
			//	
			//		minFuzzy.z = min(minFuzzy.z, fuzzyForward);
			//		maxFuzzy.z = max(maxFuzzy.z, fuzzyForward);
			//	}
			//	Vector3 FuzzyScale = maxFuzzy - minFuzzy;
			//	
			//	// Update OBB
			//	Vector3 _center = Matrix3x3(m_transform.getModel()) * _mesh->getCenter();
			//	
			//	col_OBB.position	= _center + m_transform.getWorldPosition();
			//	col_OBB.right		= m_transform.getRight()	* FuzzyScale.x;
			//	col_OBB.up			= m_transform.getUp()		* FuzzyScale.y;
			//	col_OBB.forward		= m_transform.getForward()	* FuzzyScale.z;

			//
			col_AABB = col_OBB.generateAABB();
		}
	}
}

