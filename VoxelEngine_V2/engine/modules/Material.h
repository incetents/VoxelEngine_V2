// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include "Component.h"
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"
#include "../opengl/Shader.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class BaseTexture;
	class Matrix4x4;
	class Transform;

	class MaterialPackage
	{
	public:
		MaterialPackage(const char* UniformName)
			: m_uniformName(UniformName) {}

		bool				m_exists = false;
		const std::string	m_uniformName;
		glUniform			m_uniform;
		
		bool GetUniform(ShaderProgram* shader)
		{
			assert(shader != nullptr);
			if (shader->CheckUniform(m_uniformName))
			{
				m_uniform = shader->GetUniform(m_uniformName);
				m_exists = true;
				return true;
			}
			return false;
		}
	};

	class MaterialBase
	{
		friend class Material;
	private:
		// Shader
		ShaderProgram* m_shaderProgram;
	public:
		MaterialBase(ShaderProgram* _shader);
		// Uniform Packages
		MaterialPackage Mat_viewProjection	= MaterialPackage("viewProjection");
		MaterialPackage Mat_view			= MaterialPackage("view");
		MaterialPackage Mat_projection		= MaterialPackage("projection");
		MaterialPackage Mat_camForward		= MaterialPackage("camForward");
		MaterialPackage Mat_camPosition		= MaterialPackage("camPosition");
		// Send Uniforms to shader
		void Bind();
	};

	class Material : public Component
	{
	protected:
		// Material Base
		MaterialBase* m_base;

		// Texture Package
		std::unordered_map<Active_Texture, BaseTexture*> m_textures;

		// Uniform Packages
		virtual void UpdateMaterialPackages();
		MaterialPackage Mat_model			= MaterialPackage("model");


	public:
		Material() {}

		void SetBase(MaterialBase* _base);
		void SetTexture(BaseTexture* tex, Active_Texture level);

		virtual void Bind();
	};
}