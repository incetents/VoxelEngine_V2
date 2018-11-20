// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class ShaderProgram;
	class Texture;
	class Matrix4x4;


	class Material
	{
		friend class Entity;
	private:
		ShaderProgram* m_shaderProgram;
		std::unordered_map<Active_Texture, Texture*> m_textures;

		// Uniform Package
		bool		m_hasModelMatrix;
		glUniform	m_modelMatrixUniform;
		Matrix4x4*	m_modelMatrix;

	public:
		Material(ShaderProgram* _shader);

		virtual void Bind();

		// Set Data
		void SetTexture(Texture* tex, Active_Texture level);
		void SetModelMatrix(Matrix4x4* model)
		{
			m_modelMatrix = model;
		}
	};
}