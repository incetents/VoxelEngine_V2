// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GameObject.h"

#include "../opengl/Mesh.h"
#include "../modules/Material.h"
#include "../modules/RenderManager.h"

namespace Vxl
{
	GameObject::GameObject(const std::string& name)
		: Entity(name, EntityType::GAMEOBJECT)
	{
		AddComponent(&m_material, this);
	}

	GameObject::~GameObject()
	{
		RenderManager.RemoveEntity(this);
	}

	GameObject* GameObject::Create(const std::string& name)
	{
		// Create Unique Name
		auto UniqueName = FixNameDuplicate(name);

		// Create
		GameObject* _entity = new GameObject(UniqueName);
		// Store in entity database
		AddToDatabase(UniqueName, _entity);
		Message_Created(UniqueName, _entity);
		// Return
		return _entity;
	}

	// Material
	void GameObject::SetMaterial(Material* _base)
	{
		// Do nothing if no change
		if (_base == m_material.GetBase())
			return;

		// Remove material from existing render section
		// Doesn't get called if it originally had no material base
		if (m_material.GetBase())
			RenderManager.RemoveEntity(this);

		m_material.SetBase(_base);

		RenderManager.AddEntity(this);
	}
	Material* GameObject::GetMaterial(void) const
	{
		return m_material.GetBase();
	}
	UINT GameObject::GetMaterialOrder(void) const
	{
		return m_material.GetOrder();
	}

	// Behaviour
	void GameObject::Update()
	{

	}

	void GameObject::Draw()
	{
		m_material.Bind(!m_isColoredObject);

		if(m_mesh)
			m_mesh->Draw();
	}

	void GameObject::TransformChanged()
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
}