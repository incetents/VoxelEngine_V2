// Copyright (c) 2019 Emmanuel Lajeunesse
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
		unsigned int m_blockIndex = -1;
		unsigned int m_bindingPoint = -1;
	public:
		glUniformBlock() {}
		glUniformBlock(unsigned int BlockIndex, unsigned int BindingPoint)
			: m_blockIndex(BlockIndex), m_bindingPoint(BindingPoint)
		{}
		inline unsigned int GetBlockIndex(void) const
		{
			return m_blockIndex;
		}
		inline unsigned int GetBindingPoint(void) const
		{
			return m_bindingPoint;
		}
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

		// [Faster] Set uniform
		template<typename Type>
		void Set(Type data)
		{
			assert(false);
		}

		// [Slower] Set uniform, regardless if shader is bound
		template<typename Type>
		void Set(GLuint id, Type data)
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

	//

	template<>
	void glUniform::Set<bool>(GLuint Programid, bool data);
	template<>
	void glUniform::Set<int>(GLuint Programid, int data);
	template<>
	void glUniform::Set<unsigned int>(GLuint Programid, unsigned int data);
	template<>
	void glUniform::Set<float>(GLuint Programid, float data);
	template<>
	void glUniform::Set<double>(GLuint Programid, double data);
	template<>
	void glUniform::Set<Vector2&>(GLuint Programid, Vector2& data);
	template<>
	void glUniform::Set<Vector2>(GLuint Programid, Vector2 data);
	template<>
	void glUniform::Set<Vector3&>(GLuint Programid, Vector3& data);
	template<>
	void glUniform::Set<Vector3>(GLuint Programid, Vector3 data);
	template<>
	void glUniform::Set<Vector4&>(GLuint Programid, Vector4& data);
	template<>
	void glUniform::Set<Vector4>(GLuint Programid, Vector4 data);
	template<>
	void glUniform::Set<Color3F&>(GLuint Programid, Color3F& data);
	template<>
	void glUniform::Set<Color3F>(GLuint Programid, Color3F data);
	template<>
	void glUniform::Set<Color4F&>(GLuint Programid, Color4F& data);
	template<>
	void glUniform::Set<Color4F>(GLuint Programid, Color4F data);
	template<>
	void glUniform::Set<Matrix2x2&>(GLuint Programid, Matrix2x2& data);
	template<>
	void glUniform::Set<Matrix2x2>(GLuint Programid, Matrix2x2 data);
	template<>
	void glUniform::Set<Matrix3x3&>(GLuint Programid, Matrix3x3& data);
	template<>
	void glUniform::Set<Matrix3x3>(GLuint Programid, Matrix3x3 data);
	template<>
	void glUniform::Set<Matrix4x4&>(GLuint Programid, Matrix4x4& data);
	template<>
	void glUniform::Set<Matrix4x4>(GLuint Programid, Matrix4x4 data);

}