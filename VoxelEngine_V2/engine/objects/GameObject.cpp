// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GameObject.h"

#include "../rendering/Mesh.h"
#include "../modules/Material.h"
#include "../modules/RenderManager.h"

#include "../rendering/Debug.h"
#include "../textures/Texture.h"

namespace Vxl
{
	GameObject::GameObject(const std::string& name)
		: Entity(name, EntityType::GAMEOBJECT)
	{
		//AddComponent(&m_material, this);
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
		VXL_ASSERT(_material, "Cannot Set Empty Material to GameObject");
		
		// Remove entity from existing Set
		if(m_material != nullptr)
			RenderManager.RemoveEntity(this);
		
		m_material = _material;

		// Add Entity
		RenderManager.AddEntity(this);
	}

	// Behaviour
	void GameObject::Update()
	{

	}

	void GameObject::Draw()
	{
		if (m_mesh)
		{
			// Bind Textures (ignore if wireframe mode is ON)
			if (m_material->m_Wireframe == false && !m_material->HasSharedTextures())
			{
				TextureBinder::BindTextures();
			}

			// ~ Model ~ //
			if(m_material->m_property_useModel.IsUsed())
				m_material->m_property_useModel.SetProperty(m_useTransform);

			if (m_useTransform && m_material->m_property_model.IsUsed())
			{
				m_material->m_property_model.SetPropertyMatrix(m_transform.getWorldModel(), true);
			}
			// ~ Instancing ~ //
			if (m_material->m_property_useInstancing.IsUsed())
			{
				m_material->m_property_useInstancing.SetProperty(m_mesh && m_mesh->m_instances.GetDrawCount());
			}

			// Use texture if no color and no wireframe
			bool UseTexture = !m_isColoredObject && !m_material->m_Wireframe;

			// ~ Texture ~ //
			if (m_material->m_property_useTexture.IsUsed())
			{
				m_material->m_property_useTexture.SetProperty(UseTexture);
			}

			// ~ Colors ~ //
			if (m_material->m_property_color.IsUsed())
			{
				if (!UseTexture)
					m_material->m_property_color.SetProperty(m_Color);
			}
			// ~ Tint ~ //
			if (m_material->m_property_tint.IsUsed())
			{
				m_material->m_property_tint.SetProperty(m_Tint);
			}

			// ~ ColorID ~ //
			if (m_material->m_property_output.IsUsed())
			{
				m_material->m_property_output.SetProperty(m_colorID);
			}

			// Draw Mesh
			m_mesh->Draw();
		}
	}

	void GameObject::TransformChanged()
	{
		UpdateBoundingBoxCheap(m_mesh);
	}
}