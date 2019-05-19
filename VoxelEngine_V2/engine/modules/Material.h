// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include "Component.h"
#include "../opengl/glUtil.h"
#include "../opengl/Uniform.h"
#include "../utilities/Asset.h"

#include "MaterialPackage.h"
//

#define MAX_MATERIAL_TEXTURES 8

namespace Vxl
{
	class BaseTexture;
	class Transform;
	class Texture;
	class GameObject;

	class Material : public Asset<Material>
	{
		friend class MaterialData;
		friend class RenderManager;
	protected:
		// Data
		const std::string	m_name; // Name
		ShaderProgram*		m_shaderProgram; // Shader
		UINT				m_order = 0; // Render Order

		// Locked Constructor
		Material(const std::string& _name, ShaderProgram* _shader, UINT _order)
			: m_name(_name), m_shaderProgram(_shader), m_order(_order)
		{
			UpdateMaterialPackages();
		}

		// Uniform Packages
		void UpdateMaterialPackages();

		// Gbuffer-Entity Uniforms
		MaterialPackage Mat_useModel		= MaterialPackage("VXL_useModel");
		MaterialPackage Mat_model			= MaterialPackage("VXL_model");
		MaterialPackage Mat_useInstancing	= MaterialPackage("VXL_useInstancing");
		MaterialPackage Mat_useColor		= MaterialPackage("VXL_useColor");
		MaterialPackage Mat_color			= MaterialPackage("VXL_color");
		MaterialPackage Mat_tint			= MaterialPackage("VXL_tint");

	public:
		// Database Creation
		static Material* Create(const std::string& _name, ShaderProgram* _shader, UINT _order);
		
		// Accessors
		inline std::string GetName(void) const
		{
			return m_name;
		}
		inline UINT GetOrder(void) const
		{
			return m_order;
		}

		// GL States
		Cull_Type			m_CullType		= Cull_Type::COUNTER_CLOCKWISE;
		bool				m_BlendState	= true;
		Blend_Source		m_BlendSource	= Blend_Source::SRC_ALPHA;
		Blend_Destination	m_BlendDest		= Blend_Destination::ONE_MINUS_SRC_ALPHA;
		Blend_Equation		m_BlendEq		= Blend_Equation::FUNC_ADD;
		Depth_Pass_Rule		m_DepthFunc		= Depth_Pass_Rule::LESS_OR_EQUAL;
		bool				m_DepthRead		= true;
		bool				m_DepthWrite	= true;
		bool				m_Wireframe		= false;
		
		// Send Uniforms to shader
		void Bind();

	};

	class MaterialData : public Component
	{
	private:
		friend class GameObject;
		friend class RenderManager;
	protected:
		// Material Base
		Material* m_material;
		void SetMaterial(Material* _material);

		// Texture Package
		BaseTexture* m_textures[32];
		std::set<Active_Texture> m_activeTextures;
		UINT m_activeTextureCount = 0;

		// Custom Packages
		std::map<std::string, MaterialPackage*> m_packages;
		bool CheckUniform(const std::string& uniformName);
		
	public:
		MaterialData()
		{
			memset(m_textures, 0, 32 * sizeof(BaseTexture*));
		}
		~MaterialData();

		// Custom Uniform Packages
		template<typename Type>
		void SetUniform(const std::string& uniformName, Type data)
		{
			if (m_packages.find(uniformName) == m_packages.end())
				m_packages[uniformName] = new MaterialPackageData<Type>(uniformName);
		
			if (m_packages[uniformName]->GetUniform(m_material->m_shaderProgram))
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

		inline Material* GetBase(void) const
		{
			return m_material;
		}
		inline UINT GetOrder(void) const
		{
			return m_material->m_order;
		}

		void BindUniforms(GameObject* entity);
		void BindTextures();
	};
}