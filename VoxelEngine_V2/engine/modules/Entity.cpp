// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../objects/GameObject.h"
#include "../objects/LightObject.h"
#include "../objects/CameraObject.h"

#include "../rendering/Mesh.h"
#include "../rendering/RenderManager.h"
#include "../utilities/Time.h"
#include "../utilities/Logger.h"

#include <limits>

namespace Vxl
{
	// Unique IDs
	uint32_t Entity::m_maxUniqueID = 0;
	std::stack<uint32_t> Entity::m_discardedUniqueIDs;
	std::map<uint32_t, Entity*> Entity::m_EntitiesByID;

	Entity::Entity(const std::string& name, EntityType type)
		: m_type(type)
	{
		m_name = name;
		AddComponent(&m_transform, this);

		// Check if there are discarded IDs to use
		if (!m_discardedUniqueIDs.empty())
		{
			m_uniqueID = m_discardedUniqueIDs.top();
			m_discardedUniqueIDs.pop();
		}
		// Generate new ID
		else
		{
			m_uniqueID = (++m_maxUniqueID);
		}

		// Store Self by ID
		m_EntitiesByID[m_uniqueID] = this;
		// Create ColorID
		m_colorID = Util::DataConversion::uint_to_color4(m_uniqueID);

		VXL_ASSERT(m_maxUniqueID != -1, "You've reached the maximum unique ID's for Entities (that's 2.1 billion ._.')");
	}
	Entity::~Entity()
	{
		// Remove self by ID
		m_EntitiesByID.erase(m_uniqueID);
		// Remove Unique ID
		m_discardedUniqueIDs.push(m_uniqueID);
	}

	// Rename Entity
	void Entity::Rename(const std::string& newName)
	{
		switch (m_type)
		{
		case EntityType::GAMEOBJECT:
			m_name = ((GameObject*)this)->RenameAsset(m_name, newName);
			break;
		case EntityType::LIGHT:
			m_name = ((LightObject*)this)->RenameAsset(m_name, newName);
			break;
		case EntityType::CAMERA:
			m_name = ((CameraObject*)this)->RenameAsset(m_name, newName);
			break;
		}
	}

	// Update Bounding Box from Mesh
	void Entity::UpdateBoundingBoxCheap(Mesh* _mesh)
	{
		VXL_ASSERT(_mesh, "Cannot update bounding box without mesh");

		// Update Bounding Box information
		if (_mesh != nullptr)
		{
			// Update OBB
			Vector3 _worldScale = m_transform.getWorldScale();
			Vector3 _scale = _worldScale * (_mesh->GetVertexMax() - _mesh->GetVertexMin());
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

