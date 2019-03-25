// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../opengl/Texture.h"
#include "../opengl/Shader.h"
#include "../opengl/Debug.h"
#include "../opengl/Mesh.h"

#include "../modules/Entity.h"

#include "../math/Transform.h"
#include "../math/Camera.h"

#include "../utilities/logger.h"

namespace Vxl
{
	Material::Material(
		const std::string& _name,
		ShaderProgram* _shader,
		UINT _order
	)
		: m_name(_name), m_shaderProgram(_shader), m_order(_order)
	{
		ReloadPackages();
	}

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

	void Material::ReloadPackages()
	{
		if (m_shaderProgram)
		{
			//Mat_camPosition.GetUniform(m_shaderProgram);
		}
		else
			Logger.error("Unable to load Material Packages; [Material Name] = " + m_name);

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

	// ~ Material Data ~ //

	//Texture* MaterialData::NULL_TEXTURE = nullptr;

	void MaterialData::UpdateMaterialPackages()
	{
		Mat_useModel.GetUniform(m_base->m_shaderProgram);
		Mat_model.GetUniform(m_base->m_shaderProgram);
		Mat_useInstancing.GetUniform(m_base->m_shaderProgram);
		Mat_useColorOverride.GetUniform(m_base->m_shaderProgram);
		Mat_color.GetUniform(m_base->m_shaderProgram);
		Mat_tint.GetUniform(m_base->m_shaderProgram);
	}

	void MaterialData::SetBase(Material* _base)
	{
		if (m_base == _base)
			return;

		m_base = _base;
		UpdateMaterialPackages();
		RemoveAllUniforms();
	}

	MaterialData::~MaterialData()
	{
		RemoveAllUniforms();
		RemoveAllUniforms();
	}

	bool MaterialData::CheckUniform(const std::string& uniformName)
	{
		// Check if shader contains uniform
		return (m_base->m_shaderProgram->CheckUniform(uniformName));
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
		{
			delete it->second;
		}
		
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

	void MaterialData::Bind(bool BindTextures)
	{
		assert(m_base);
		// Bind Uniforms for this object

		// ~ Model ~ //
		if (Mat_useModel.m_exists)
		{
			Mat_useModel.m_uniform.Set<bool>(m_owner->m_useTransform);
		}
		if (m_owner->m_useTransform && Mat_model.m_exists)
		{
			Mat_model.m_uniform.Set(m_owner->m_transform.getModel());
		}
		// ~ Special Setters ~ //
		if (Mat_useInstancing.m_exists && m_owner->GetMesh())
		{
			Mat_useInstancing.m_uniform.Set<bool>(m_owner->GetMesh()->m_instances.GetDrawCount() > 0 ? true : false);
		}
		if (Mat_useColorOverride.m_exists)
		{
			// Color info or wireframe mode will cause color to be used in shader
			Mat_useColorOverride.m_uniform.Set<bool>(m_owner->m_isColoredObject || m_base->m_Wireframe);
			if ((m_owner->m_isColoredObject || m_base->m_Wireframe) && Mat_color.m_exists)
				Mat_color.m_uniform.Set<Color3F>(m_owner->GetColor());
		}
		if (Mat_tint.m_exists)
		{
			Mat_tint.m_uniform.Set<Color3F>(m_owner->GetTint());
		}

		// Custom Uniforms //
		for (auto it = m_packages.begin(); it != m_packages.end(); it++)
		{
			it->second->SendDataAsUniform();
		}
		
		// Bind Textures (ignore if parameter is OFF or wireframe mode is ON)
		if (BindTextures && m_base->m_Wireframe == false)
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