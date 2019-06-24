// Copyright(c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"
#include "../modules/Material.h"

namespace Vxl
{
	class Mesh;

	class GameObject : public Entity, public Asset<GameObject>, public TextureBinder
	{
	private:
		GameObject(const std::string& name);

		// Data
		Mesh*			m_mesh = nullptr;
		Material*		m_material = nullptr;

	public:
		// Destructor
		~GameObject();

		// Database Creation
		static GameObject* Create(const std::string& name);

		// Mesh
		inline void SetMesh(Mesh* _mesh)
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
		void SetMaterial(Material* _material);
		inline Material* GetMaterial(void) const
		{
			return m_material;
		}


		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}