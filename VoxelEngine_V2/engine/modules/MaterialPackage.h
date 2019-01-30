// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <assert.h>

#include "../opengl/Shader.h"
#include "../opengl/Uniform.h"

namespace Vxl
{
	class ShaderProgram;

	// Store Uniform Info
	class MaterialPackage
	{
	public:
		MaterialPackage(const std::string& UniformName)
			: m_uniformName(UniformName) {}

		bool				m_exists = false;
		const std::string	m_uniformName;
		glUniform			m_uniform;

		bool GetUniform(ShaderProgram* shader)
		{
			if (!shader)
				return false;

			//assert(shader != nullptr);
			if (shader->CheckUniform(m_uniformName))
			{
				m_uniform = shader->GetUniform(m_uniformName);
				m_exists = true;
				return true;
			}
			m_exists = false;
			return false;
		}
		virtual void SendDataAsUniform()
		{
			// Should only be called from derived class
			assert(false);
		}
	};

	template<typename Type>
	class MaterialPackageData : public MaterialPackage
	{
	public:
		MaterialPackageData(const std::string& UniformName)
			: MaterialPackage(UniformName) {}

		Type m_data;

		void SendDataAsUniform() override
		{
			if (m_exists)
				m_uniform.Set<Type>(m_data);
		}
	};

}
