// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "GameObject.h"

#include "../rendering/Mesh.h"
#include "../rendering/Debug.h"

#include "../modules/Material.h"
#include "../rendering/RenderManager.h"

#include "../textures/Texture2D.h"

namespace Vxl
{
	GameObject::GameObject(const std::string& name)
		: Entity(name, EntityType::GAMEOBJECT)
	{
		
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
		AddNamedAsset(UniqueName, _entity, AssetMessage::CREATED);
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
	void GameObject::Update(void)
	{

	}

	void GameObject::DrawSelection(void)
	{
		static Vector3 Epsilon = Vector3(0.01f, 0.01f, 0.01f);

		Color4F AABB_Color = IsFamilyActive() ? Color4F::YELLOW : Color4F::GREY;
		Color4F OBB_Color = IsFamilyActive() ? Color4F::GREEN : Color4F::GREY;

		if (m_mesh->m_instances.Empty())
		{
			Debug.DrawLineOBB(*this, m_transform.getWorldPosition(), 3.0f, OBB_Color);
			Debug.DrawLineAABB(GetAABBMin() - Epsilon, GetAABBMax() + Epsilon, m_transform.getWorldPosition(), 3.0f, AABB_Color);
		}
		// Draw outline around all instances
		else
		{
			//Vector4 WPosition = Vector4(Entity->m_transform.getWorldPosition(), 1);
			auto Instances = m_mesh->m_instances.GetVertices();
			auto _model = m_transform.getModel();
			for (Matrix4x4& instanceMatrix : *Instances)
			{
				Vector4 Instance_Pos = Vector4(instanceMatrix[12], instanceMatrix[13], instanceMatrix[14], 1);

				Vector3 PosWorld = Vector3(_model * Instance_Pos);

				Debug.DrawLineOBB(*this, PosWorld, 3.0f, OBB_Color);
				Debug.DrawLineAABB(GetAABBMin() - Epsilon, GetAABBMax() + Epsilon, PosWorld, 3.0f, AABB_Color);
			}
		}
	}

	void GameObject::Draw(void)
	{
		if (m_mesh && m_material->IsValid())
		{
			// Bind Textures (ignore if wireframe mode is ON)
			if (m_material->m_Wireframe == false && !m_material->HasSharedTextures())
			{
				TextureBinder::BindTextures();
			}

			// ~ Model ~ //
			if (m_material->m_property_useModel.IsUsed())
			{
				m_material->m_property_useModel.SetProperty(m_useTransform);

				if (m_useTransform)
				{
					if(m_material->m_property_model.IsUsed())
						m_material->m_property_model.SetPropertyMatrix(m_transform.getModel(), true);

					if(m_material->m_property_normalMatrix.IsUsed())
						m_material->m_property_normalMatrix.SetPropertyMatrix(m_transform.getNormalMatrix(), true);
				}
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

			// ~ Alpha ~ //
			if (m_material->m_property_alpha.IsUsed())
			{
				if(m_material->GetRenderMode() == MaterialRenderMode::Transparent)
					m_material->m_property_alpha.SetProperty(m_alpha);
				else
					m_material->m_property_alpha.SetProperty(1.0f);
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