// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include "Component.h"
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"

#include "MaterialPackage.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class BaseTexture;
	class Transform;

	class MaterialBase
	{
		friend class Material;
	protected:
		// Shader
		ShaderProgram* m_shaderProgram;
		// Render Order
		UINT m_order = 0;
	public:
		MaterialBase(ShaderProgram* _shader, UINT _order);
		// Uniform Packages
		MaterialPackage Mat_viewProjection	= MaterialPackage("VXL_viewProjection");
		MaterialPackage Mat_view			= MaterialPackage("VXL_view");
		MaterialPackage Mat_projection		= MaterialPackage("VXL_projection");
		MaterialPackage Mat_camForward		= MaterialPackage("VXL_camForward");
		MaterialPackage Mat_camPosition		= MaterialPackage("VXL_camPosition");
		// Send Uniforms to shader
		virtual void Bind();
		// Wireframe
		bool m_wireframe = false;
	};

	class Material : public Component
	{
		enum DataType
		{
			Vec3 = 0
		};

		friend class Entity;
	protected:
		// Material Base
		MaterialBase* m_base;
		void SetBase(MaterialBase* _base);

		// Texture Package
		BaseTexture* m_textures[32];
		std::set<Active_Texture> m_activeTextures;

		// Uniform Packages
		virtual void UpdateMaterialPackages();
		// Gbuffer-Entity Uniforms
		MaterialPackage Mat_useModel			= MaterialPackage("VXL_useModel");
		MaterialPackage Mat_model				= MaterialPackage("VXL_model");
		MaterialPackage Mat_useInstancing		= MaterialPackage("VXL_useInstancing");
		MaterialPackage Mat_useColorOverride	= MaterialPackage("VXL_useColorOverride");
		MaterialPackage Mat_color				= MaterialPackage("VXL_color");
		MaterialPackage Mat_tint				= MaterialPackage("VXL_tint");
		// Custom Packages
		std::unordered_map<std::string, MaterialPackage*> m_packages;
		bool CheckUniform(const std::string& uniformName);
		
	public:
		Material() {}
		~Material();

		// Custom Uniform Packages
		template<typename Type>
		void SetUniform(const std::string& uniformName, Type data)
		{
			if (m_packages.find(uniformName) == m_packages.end())
				m_packages[uniformName] = new MaterialPackageData<Type>(uniformName);
		
			if (m_packages[uniformName]->GetUniform(m_base->m_shaderProgram))
			{
				auto P = dynamic_cast<MaterialPackageData<Type>*>(m_packages[uniformName]);
				assert(P);
				P->m_data = data;
			}
		}
		void RemoveUniform(const std::string& uniformName);
		void RemoveAllUniforms();

		void SetTexture(BaseTexture* tex, Active_Texture level);
		void ClearTexture(Active_Texture level);

		inline MaterialBase* GetBase(void) const
		{
			return m_base;
		}
		inline UINT GetOrder(void) const
		{
			return m_base->m_order;
		}

		virtual void Bind(bool BindTextures);
	};
}