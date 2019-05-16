// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"
#include "../modules/Material.h"

namespace Vxl
{
	class Mesh;
	class Material;

	class GameObject : public Entity
	{
	private:
		GameObject(const std::string& name);

	public:
		// Destructor
		~GameObject();

		// Database Creation
		static GameObject* Create(const std::string& name);

		// Data
		Mesh*			m_mesh = nullptr;
		MaterialData	m_material;

		// Mesh
		inline Mesh* GetMesh(void) const
		{
			return m_mesh;
		}
		inline void SetMesh(Mesh* _mesh)
		{
			m_mesh = _mesh;
			// call callbacks since transform might change
			TransformChanged();
		}
	
		// Material
		void			SetMaterial(Material* _base);
		Material*		GetMaterial(void) const;
		UINT			GetMaterialOrder(void) const;



		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}