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

		void set(const std::string& UniformName, const std::string& FunctionName);
		void bind();
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

		void set(GLfloat* arr, GLuint arrLength, size_t startOffset = 0);
	};

	class glUniform
	{
	private:
		GLuint m_location = -1;

	public:
		glUniform(GLuint location = -1) : m_location(location) {}
		glUniform(const glUniform& U) : m_location(U.m_location) {}
		glUniform& operator=(const glUniform& U) { m_location = U.m_location; return *this; }

		template<typename Type>
		void set(Type data)
		{
			assert(false);
		}

	};

	template<>
	void glUniform::set<bool>(bool data);
	template<>
	void glUniform::set<int>(int data);
	template<>
	void glUniform::set<unsigned int>(unsigned int data);
	template<>
	void glUniform::set<float>(float data);
	template<>
	void glUniform::set<double>(double data);
	template<>
	void glUniform::set<Vector2&>(Vector2& data);
	template<>
	void glUniform::set<Vector2>(Vector2 data);
	template<>
	void glUniform::set<Vector3&>(Vector3& data);
	template<>
	void glUniform::set<Vector3>(Vector3 data);
	template<>
	void glUniform::set<Vector4&>(Vector4& data);
	template<>
	void glUniform::set<Vector4>(Vector4 data);
	template<>
	void glUniform::set<Matrix2x2&>(Matrix2x2& data);
	template<>
	void glUniform::set<Matrix2x2>(Matrix2x2 data);
	template<>
	void glUniform::set<Matrix3x3&>(Matrix3x3& data);
	template<>
	void glUniform::set<Matrix3x3>(Matrix3x3 data);
	template<>
	void glUniform::set<Matrix4x4&>(Matrix4x4& data);
	template<>
	void glUniform::set<Matrix4x4>(Matrix4x4 data);
	//template<>
	//void Uniform::setUniform<Texture>(Texture& data);

}