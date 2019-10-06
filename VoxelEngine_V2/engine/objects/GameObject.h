// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"
#include "../modules/Material.h"

namespace Vxl
{
	class Mesh;

	class GameObject : public Entity, public Asset<GameObject>, public TextureBinder
	{
	protected:
		GameObject(const std::string& name);

		// Data
		Mesh*			m_mesh = nullptr;
		Material*		m_material = nullptr;

	public:
		// Destructor
		~GameObject();

		// Asset Creation
		static GameObject* Create(const std::string& name);

		// Mesh
		virtual void SetMesh(Mesh* _mesh)
		{
			m_mesh = _mesh;
			// call callbacks since transform might change
			TransformChanged();
		}
		inline Mesh* GetMesh(void) const
		{
			return m_mesh;
		}
	
		// Material
		virtual void SetMaterial(Material* _material);
		inline Material* GetMaterial(void) const
		{
			return m_material;
		}

		// Behaviour
		virtual void Update(void) override;
		virtual void DrawSelection(void) override;
		virtual void Draw(void) override;
	protected:
		virtual void TransformChanged(void) override;
	};
}