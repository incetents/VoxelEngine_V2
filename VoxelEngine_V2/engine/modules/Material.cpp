// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../opengl/Shader.h"
#include "../opengl/Texture.h"
#include "../math/Matrix4x4.h"

namespace Vxl
{
	Material::Material(ShaderProgram* _shader)
	{
		m_shaderProgram = _shader;

		m_hasModelMatrix = m_shaderProgram->CheckUniform("model");
		
		if(m_hasModelMatrix)
			m_modelMatrixUniform = m_shaderProgram->GetUniform("model");
	}

	void Material::SetTexture(Texture* tex, Active_Texture level)
	{
		assert((int)level < MAX_MATERIAL_TEXTURES);
		m_textures[level] = tex;
	}

	void Material::Bind()
	{
		// Bind Shader

		// Bind Uniforms
		if (m_hasModelMatrix && m_modelMatrix)
			m_modelMatrixUniform.set(*m_modelMatrix);

		// Bind Textures
		for (auto it = m_textures.begin(); it != m_textures.end(); it++)
		{
			glUtil::setActiveTexture(it->first);
			it->second->Bind();
		}
	}
}