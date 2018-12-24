// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../opengl/Shader.h"
#include "../opengl/Texture.h"
#include "../math/Matrix4x4.h"
#include "../math/Transform.h"

namespace Vxl
{
	void Material::UpdateMaterialPackages()
	{
		if (m_shaderProgram->CheckUniform("model"))
		{
			Mat_modelMatrix.m_uniform = m_shaderProgram->GetUniform("model");
			Mat_modelMatrix.m_exists = true;
		}
	}

	void Material::SetShader(ShaderProgram* _shader)
	{
		m_shaderProgram = _shader;
		UpdateMaterialPackages();
	}

	void Material::SetTexture(Texture* tex, Active_Texture level)
	{
		assert((int)level - GL_TEXTURE0 < MAX_MATERIAL_TEXTURES);
		m_textures[level] = tex;
	}


	void Material::SetTransformReference(Transform* _transform)
	{
		Mat_modelMatrix.m_data = _transform;
	}


	void Material::Bind()
	{
		// Bind Uniforms
		if (Mat_modelMatrix.m_data != nullptr)
		{
			Mat_modelMatrix.m_uniform.set(Mat_modelMatrix.m_data->getModel());
		}

		// Bind Textures
		for (auto it = m_textures.begin(); it != m_textures.end(); it++)
		{
			glUtil::setActiveTexture(it->first);
			it->second->Bind();
		}
	}
}