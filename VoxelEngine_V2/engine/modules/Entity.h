// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Component.h"
#include "../math/Transform.h"
#include "../math/Color.h"
#include "../opengl/MeshRenderer.h"
#include "../modules/Material.h"

namespace Vxl
{
	class Mesh;
	class Material;
	class MaterialBase;
	class ShaderProgram;

	class Entity : public ComponentHandler
	{
	public:
		Entity();
		virtual ~Entity();

		// Data
		Transform m_transform;
		bool m_useTransform = true;
		MeshRenderer m_meshRenderer;
		Material m_material;
		
		// Mesh Rendering
		bool m_isActive = true;
		bool m_isColoredObject = false;
		Mesh* m_mesh;

		// Utility
		void			SetMaterialBase(MaterialBase* _base);
		MaterialBase*	GetMaterialBase(void) const;
		UINT			GetMaterialOrder(void) const;

		// Color
		void SetColor(Color3F color, bool enableIsColoredObject = true)
		{
			m_Color = color;
			m_isColoredObject = enableIsColoredObject;
		}
		Color3F& GetColor(void)
		{
			return m_Color;
		}

		void SetTint(Color3F tint)
		{
			m_Tint = tint;
		}
		Color3F GetTint(void)
		{
			return m_Tint;
		}

		// Behaviour
		virtual void Update();
		virtual void Draw();

	protected:
		Color3F m_Color = Color3F(1, 1, 1);
		Color3F m_Tint = Color3F(1, 1, 1);

	};
}

