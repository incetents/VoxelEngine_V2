// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include "Component.h"
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class ShaderProgram;
	class Texture;
	class Matrix4x4;
	class Transform;

	template<typename Type>
	class MaterialPackage
	{
	public:
		bool		m_exists = false;
		Type*		m_data	= nullptr;
		glUniform	m_uniform;
	};

	class Material : public Component
	{
	protected:
		// Shader
		ShaderProgram* m_shaderProgram;

		// Texture Package
		std::unordered_map<Active_Texture, Texture*> m_textures;

		// Uniform Package
		virtual void UpdateMaterialPackages();
		MaterialPackage<Transform> Mat_modelMatrix;

	public:
		Material() {}

		void SetShader(ShaderProgram* _shader);
		void SetTexture(Texture* tex, Active_Texture level);
		void SetTransformReference(Transform* _transform);

		virtual void Bind();
	};
}