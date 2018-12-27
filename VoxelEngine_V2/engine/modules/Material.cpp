// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../opengl/Texture.h"
#include "../opengl/TextureTracker.h"
#include "../math/Matrix4x4.h"
#include "../math/Transform.h"
#include "../math/Camera.h"
#include "../modules/Entity.h"

namespace Vxl
{
	void Material::UpdateMaterialPackages()
	{
		Mat_model.GetUniform(m_shaderProgram);
		Mat_viewProjection.GetUniform(m_shaderProgram);
		Mat_view.GetUniform(m_shaderProgram);
		Mat_projection.GetUniform(m_shaderProgram);
		Mat_camForward.GetUniform(m_shaderProgram);
		Mat_camPosition.GetUniform(m_shaderProgram);
	}

	void Material::SetShader(ShaderProgram* _shader)
	{
		m_shaderProgram = _shader;
		UpdateMaterialPackages();
	}

	void Material::SetTexture(BaseTexture* tex, Active_Texture level)
	{
		assert((int)level - GL_TEXTURE0 < MAX_MATERIAL_TEXTURES);
		m_textures[level] = tex;
	}

	void Material::Bind()
	{
		// Bind Uniforms

		// ~ Model ~ //
		if (Mat_model.m_exists)
		{
			Mat_model.m_uniform.set(m_owner->m_transform.getModel());
		}
		// ~ View Projection ~ //
		if (Mat_viewProjection.m_exists)
		{
			Mat_viewProjection.m_uniform.set<Matrix4x4>(Camera::GetMain()->getViewProjection());
		}
		// ~ View ~ //
		if (Mat_view.m_exists)
		{
			Mat_view.m_uniform.set<Matrix4x4>(Camera::GetMain()->getView());
		}
		// ~ Projection ~ //
		if (Mat_projection.m_exists)
		{
			Mat_projection.m_uniform.set<Matrix4x4>(Camera::GetMain()->getProjection());
		}
		// ~ CamForward ~ //
		if (Mat_camForward.m_exists)
		{
			Mat_camForward.m_uniform.set<Vector3>(Camera::GetMain()->getForward());
		}
		// ~ CamPosition ~ //
		if (Mat_camPosition.m_exists)
		{
			Mat_camPosition.m_uniform.set<Vector3>(Camera::GetMain()->getPosition());
		}

		// Bind Textures
		for (auto it = m_textures.begin(); it != m_textures.end(); it++)
		{
			glUtil::setActiveTexture(it->first);

			// Don't bind texture on active layer if it's already bound
			if (TextureTracker.ShouldBindTexture(it->first, it->second->GetID()))
			{
				it->second->Bind();
				TextureTracker.SetCurrentTexture(it->first, it->second->GetID());
			}
		}
	}
}