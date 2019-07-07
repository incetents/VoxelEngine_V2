// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <assert.h>
#include <string>
#include <unordered_map>

#include "Graphics.h"

#include "../utilities/Macros.h"

namespace Vxl
{
	enum class ShaderType;
	class Shader;
	class Vector2;
	class Vector3;
	class Vector4;
	class Color3F;
	class Color4F;
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;

	class glSubroutine
	{
		friend class ShaderProgram;
	private:
		ShaderType	m_type;
		uint32_t*		m_locations = nullptr;
		uint32_t		m_locationCount = 0;
		std::unordered_map<std::string, uint32_t> m_uniforms; // subroutine names
		std::unordered_map<std::string, uint32_t> m_functions; // subroutine functions
	public:
		glSubroutine() {}
		glSubroutine(Shader* shader, uint32_t subroutine_count);
		~glSubroutine()
		{
			if (m_locations)
				delete[] m_locations;
		}

		void Set(const std::string& UniformName, const std::string& FunctionName);
		void Bind();
	};

}