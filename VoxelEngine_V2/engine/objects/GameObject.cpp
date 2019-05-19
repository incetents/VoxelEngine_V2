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
	void GameObject::SetMaterial(Material* _material)
	{
		// Do nothing if giving same material
		if (_material == m_material.GetBase())
			return;

		// Remove material from existing render section
		// Doesn't get called if it originally had no material base
		if (m_material.GetBase())
			RenderManager.RemoveEntity(this);

		m_material.SetMaterial(_material);

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
		if (m_mesh)
		{
			// Send Uniform Data
			m_material.BindUniforms(this);

			// Send Texture Data
			if (!m_isColoredObject)
				m_material.BindTextures();

			// Draw Mesh
			m_mesh->Draw();
		}
	}

	void GameObject::TransformChanged()
	{
		UpdateBoundingBoxCheap(m_mesh);
	}
}