// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../opengl/Texture.h"
#include "../opengl/Shader.h"
#include "../opengl/Debug.h"
#include "../opengl/Mesh.h"

#include "../modules/Entity.h"
#include "../objects/GameObject.h"

#include "../math/Transform.h"

#include "../utilities/logger.h"

namespace Vxl
{
	Material* Material::Create(
		const std::string& _name,
		ShaderProgram* _shader,
		UINT _order
	){
		Material* _material = new Material(_name, _shader, _order);

		AddToDatabase(_name, _material);
		Message_Created(_name, _material);

		return _material;
	}

	void Material::Bind()
	{
		if (!m_shaderProgram)
			return;

		// Bind Shader
		m_shaderProgram->Bind();

		// Bind Modes
		glUtil::cullMode(m_CullType);
		glUtil::blendState(m_BlendState);
		if (m_BlendState)
		{
			glUtil::blendMode(m_BlendSource, m_BlendDest);
			glUtil::blendEquation(m_BlendEq);
		}
		glUtil::depthTest(m_DepthFunc);
		glUtil::depthState(m_DepthRead);
		glUtil::depthMask(m_DepthWrite);
		glUtil::wireframe(m_Wireframe);
	}

	void Material::UpdateMaterialPackages()
	{
		Mat_useModel.GetUniform(m_shaderProgram);
		Mat_model.GetUniform(m_shaderProgram);
		Mat_useInstancing.GetUniform(m_shaderProgram);
		Mat_useColor.GetUniform(m_shaderProgram);
		Mat_color.GetUniform(m_shaderProgram);
		Mat_tint.GetUniform(m_shaderProgram);
	}

	// ~ Material Data ~ //

	void MaterialData::SetMaterial(Material* _material)
	{
		if (m_material == _material)
			return;

		m_material = _material;
		RemoveAllUniforms();
	}

	

	MaterialData::~MaterialData()
	{
		RemoveAllUniforms();
	}

	bool MaterialData::CheckUniform(const std::string& uniformName)
	{
		// Check if shader contains uniform
		return (m_material->m_shaderProgram->CheckUniform(uniformName));
	}

	void MaterialData::RemoveUniform(const std::string& uniformName)
	{
		// Check if name exists
		if (m_packages.find(uniformName) != m_packages.end())
		{
			delete m_packages[uniformName];
			m_packages.erase(uniformName);
		}
	}

	void MaterialData::RemoveAllUniforms()
	{
		for (auto it = m_packages.begin(); it != m_packages.end(); it++)
			delete it->second;
		
		m_packages.clear();
	}

	void MaterialData::SetTexture(BaseTexture* tex, Active_Texture level)
	{
		assert((int)level - GL_TEXTURE0 < MAX_MATERIAL_TEXTURES);
		m_textures[(UINT)level - GL_TEXTURE0] = tex;
		if (tex)
			m_activeTextures.insert(level);
		else
			m_activeTextures.erase(level);

		m_activeTextureCount = (UINT)m_activeTextures.size();
	}

	void MaterialData::ClearTexture(Active_Texture level)
	{
		m_activeTextures.erase(level);
		m_activeTextureCount = (UINT)m_activeTextures.size();
	}

	void MaterialData::BindUniforms(GameObject* entity)
	{
		assert(m_material);
		// Bind Uniforms for this object

		// ~ Model ~ //
		if (m_material->Mat_useModel.m_exists)
		{
			m_material->Mat_useModel.m_uniform.Set<bool>(m_owner->m_useTransform);
		}
		if (m_owner->m_useTransform && m_material->Mat_model.m_exists)
		{
			m_material->Mat_model.m_uniform.Set(m_owner->m_transform.getModel());
		}
		// ~ Special Setters ~ //
		if (m_material->Mat_useInstancing.m_exists)
		{
			if (entity->GetMesh())
				m_material->Mat_useInstancing.m_uniform.Set<bool>(entity->GetMesh()->m_instances.GetDrawCount() > 0 ? true : false);
			else
				m_material->Mat_useInstancing.m_uniform.Set<bool>(false);
		}
		if (m_material->Mat_useColor.m_exists)
		{
			// Color info or wireframe mode will cause color to be used in shader
			m_material->Mat_useColor.m_uniform.Set<bool>(m_owner->m_isColoredObject || m_material->m_Wireframe);
			if ((m_owner->m_isColoredObject || m_material->m_Wireframe) && m_material->Mat_color.m_exists)
				m_material->Mat_color.m_uniform.Set<Color3F>(m_owner->GetColor());
		}
		if (m_material->Mat_tint.m_exists)
		{
			m_material->Mat_tint.m_uniform.Set<Color3F>(m_owner->GetTint());
		}

		// Custom Uniforms //
		for (auto it = m_packages.begin(); it != m_packages.end(); it++)
		{
			it->second->SendDataAsUniform();
		}

	}

	void MaterialData::BindTextures()
	{
		assert(m_material);

		// Bind Textures (ignore if parameter is OFF or wireframe mode is ON)
		if (m_material->m_Wireframe == false)
		{
			// Null Texture if no textures are used
			if (!m_activeTextureCount)
			{
				// Bind null texture on first active layer
				Debug.GetNullTexture()->Bind(Active_Texture::LEVEL0);
			}
			else
			{
				for (Active_Texture id : m_activeTextures)
				{
					// Get Current texture
					BaseTexture* _tex = m_textures[(UINT)id - GL_TEXTURE0];

					// Bind Null texture if texture isn't loaded
					if (_tex == nullptr || !_tex->IsLoaded())
					{
						Debug.GetNullTexture()->Bind(Active_Texture::LEVEL0);
					}
					// Bind texture normally
					else
					{
						_tex->Bind(id);
					}
				}
			}
		}
	}
}