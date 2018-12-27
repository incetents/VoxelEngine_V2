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

	class Material : public Component
	{
	protected:
		// Shader
		ShaderProgram* m_shaderProgram;

		// Texture Package
		std::unordered_map<Active_Texture, BaseTexture*> m_textures;

		// Uniform Packages
		virtual void UpdateMaterialPackages();
		MaterialPackage Mat_model			= MaterialPackage("model");
		MaterialPackage Mat_viewProjection	= MaterialPackage("viewProjection");
		MaterialPackage Mat_view			= MaterialPackage("view");
		MaterialPackage Mat_projection		= MaterialPackage("projection");
		MaterialPackage Mat_camForward		= MaterialPackage("camForward");
		MaterialPackage Mat_camPosition		= MaterialPackage("camPosition");

	public:
		Material() {}

		void SetShader(ShaderProgram* _shader);
		void SetTexture(BaseTexture* tex, Active_Texture level);

		virtual void Bind();
	};
}