// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GameObject.h"

#include "../opengl/Mesh.h"
#include "../modules/Material.h"
#include "../modules/RenderManager.h"

#include "../opengl/Debug.h"
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

			// Set Material Properties
			auto p = ShaderProgram::GetBoundProgram();

			// ~ Model ~ //
			if(m_material->m_property_useModel.IsUsed())
				m_material->m_property_useModel.SetProperty(m_useTransform);

			if (m_useTransform && m_material->m_property_model.IsUsed())
			{
				m_material->m_property_model.SetProperty(m_transform.getWorldModel().Transpose());
			}
			// ~ Instancing ~ //
			if (m_material->m_property_useInstancing.IsUsed())
			{
				m_material->m_property_useInstancing.SetProperty(m_mesh && m_mesh->m_instances.GetDrawCount());
			}

			// ~ Colors ~ //
			if (m_material->m_property_color.IsUsed() && m_material->m_property_color.IsUsed())
			{
				// Color info or wireframe mode will cause color to be used in shader
				m_material->m_property_useColor.SetProperty(m_isColoredObject || m_material->m_Wireframe);

				if ((m_isColoredObject || m_material->m_Wireframe))
					m_material->m_property_color.SetProperty(m_Color);
			}
			// ~ Tint ~ //
			if (m_material->m_property_tint.IsUsed())
			{
				m_material->m_property_tint.SetProperty(m_Tint);
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