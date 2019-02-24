// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include <assert.h>
#include <string>
#include <unordered_map>

#include "glUtil.h"

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
		GLuint*		m_locations = nullptr;
		GLuint		m_locationCount = 0;
		std::unordered_map<std::string, GLuint> m_uniforms; // subroutine names
		std::unordered_map<std::string, GLuint> m_functions; // subroutine functions
	public:
		glSubroutine() {}
		glSubroutine(Shader* shader, GLuint subroutine_count);
		~glSubroutine()
		{
			if (m_locations)
				delete[] m_locations;
		}

		void Set(const std::string& UniformName, const std::string& FunctionName);
		void Bind();
	};

	class glUniformBlock
	{
	private:
		GLfloat* m_array = nullptr;
		GLuint   m_bufferSize;
		GLuint	 m_bindingPoint;
		GLuint	 m_blockIndex;
		GLuint	 m_buffer;

	public:
		glUniformBlock() {}
		glUniformBlock(const ShaderProgram& program, const std::string& blockName, GLuint bindingPoint, GLuint bufferSize);
		~glUniformBlock();

		void Set(GLfloat* arr, GLuint arrLength, size_t startOffset = 0);
	};

	class glUniform
	{
	private:
		GLuint m_location = -1;
		GLenum m_type = 0;

	public:
		glUniform(GLuint location = -1, GLenum type = 0)
			: m_location(location), m_type(type) {}

		glUniform(const glUniform& U)
			: m_location(U.m_location), m_type(U.m_type) {}

		glUniform& operator=(const glUniform& U)
		{
			m_location = U.m_location;
			m_type = U.m_type;
			return *this;
		}

		template<typename Type>
		void Set(Type data)
		{
			assert(false);
		}

		inline GLenum GetLocation(void) const
		{
			return m_location;
		}
		inline GLenum GetType(void) const
		{
			return m_type;
		}
	};

	template<>
	void glUniform::Set<bool>(bool data);
	template<>
	void glUniform::Set<int>(int data);
	template<>
	void glUniform::Set<unsigned int>(unsigned int data);
	template<>
	void glUniform::Set<float>(float data);
	template<>
	void glUniform::Set<double>(double data);
	template<>
	void glUniform::Set<Vector2&>(Vector2& data);
	template<>
	void glUniform::Set<Vector2>(Vector2 data);
	template<>
	void glUniform::Set<Vector3&>(Vector3& data);
	template<>
	void glUniform::Set<Vector3>(Vector3 data);
	template<>
	void glUniform::Set<Vector4&>(Vector4& data);
	template<>
	void glUniform::Set<Vector4>(Vector4 data);
	template<>
	void glUniform::Set<Color3F&>(Color3F& data);
	template<>
	void glUniform::Set<Color3F>(Color3F data);
	template<>
	void glUniform::Set<Color4F&>(Color4F& data);
	template<>
	void glUniform::Set<Color4F>(Color4F data);
	template<>
	void glUniform::Set<Matrix2x2&>(Matrix2x2& data);
	template<>
	void glUniform::Set<Matrix2x2>(Matrix2x2 data);
	template<>
	void glUniform::Set<Matrix3x3&>(Matrix3x3& data);
	template<>
	void glUniform::Set<Matrix3x3>(Matrix3x3 data);
	template<>
	void glUniform::Set<Matrix4x4&>(Matrix4x4& data);
	template<>
	void glUniform::Set<Matrix4x4>(Matrix4x4 data);

}