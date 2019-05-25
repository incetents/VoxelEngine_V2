// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../opengl/glUtil.h"
#include "../opengl/Mesh.h"
#include "../modules/RenderManager.h"
#include "../utilities/Time.h"
#include "../utilities/logger.h"

#include <limits>

namespace Vxl
{
	Entity::Entity(const std::string& name, EntityType type)
		: m_type(type)
	{
		SetName(name);
		AddComponent(&m_transform, this);
	}

	void Entity::SetName(const std::string _name)
	{
		if (_name.size() > MAX_ENTITY_NAME_LENGTH)
		{
			Logger.error("[Entity Error]");
			Logger.error("New name is too large,\n[Orig Name]: " + m_name + "\n[New Name]: " + _name);
			m_name = _name.substr(0, MAX_ENTITY_NAME_LENGTH);
		}
		else
			m_name = _name;
	}

	// Update Bounding Box from Mesh
	void Entity::UpdateBoundingBoxCheap(Mesh* _mesh)
	{
		assert(_mesh);

		// Update Bounding Box information
		if (_mesh != nullptr)
		{
			// Update OBB 
			Vector4 VMin = Vector4(_mesh->GetVertexMin(), 1);
			Vector4 VMax = Vector4(_mesh->GetVertexMax(), 1);
			Matrix4x4 Model = m_transform.getWorldModel();
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
}

