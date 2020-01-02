//// Copyright(c) 2020 Emmanuel Lajeunesse
//#pragma once
//
//#include "../modules/Entity.h"
//#include "../modules/Material.h"
//
//namespace Vxl
//{
//	class Mesh;
//
//	class GameObject : public Entity //, public Asset<GameObject>
//	{
//		DISALLOW_COPY_AND_ASSIGN(GameObject);
//		friend class _Assets;
//	protected:
//		GameObject(const std::string& name);
//
//		// Material
//		MaterialIndex	m_material = -1;
//
//	public:
//		// Destructor
//		~GameObject();
//
//		// Asset Creation
//		//static GameObject* Create(const std::string& name);
//
//		// Mesh
//		MeshIndex		m_mesh = -1;
//
//		// Material
//		void SetMaterial(MaterialIndex index);
//
//		// Behaviour
//		virtual void Update(void) override;
//		virtual void DrawSelection(void) override;
//		virtual void Draw(void) override;
//	protected:
//		virtual void TransformChanged(void) override;
//	};
//}