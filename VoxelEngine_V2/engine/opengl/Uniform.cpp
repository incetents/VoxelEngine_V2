// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Uniform.h"

#include "Shader.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

namespace Vxl
{
	glSubroutine::glSubroutine(Shader* shader, GLuint subroutine_count)
	{
		m_type = shader->GetType();
		m_locationCount = subroutine_count;
		m_locations = new GLuint[subroutine_count];
		memset(m_locations, 0, subroutine_count * sizeof(GLuint));
	}

	void glSubroutine::Set(const std::string& UniformName, const std::string& FunctionName)
	{
		if (m_uniforms.find(UniformName) != m_uniforms.end() && m_functions.find(FunctionName) != m_functions.end())
		{
			GLuint index = m_uniforms[UniformName];
			GLuint functionIndex = m_functions[FunctionName];
			m_locations[index] = functionIndex;
			return;
		}
		assert(FALSE);
	}

	void glSubroutine::Bind()
	{
		glUniformSubroutinesuiv((GLenum)m_type, m_locationCount, m_locations);
	}

	template<>
	void glUniform::Set<bool>(bool data)
	{
		glUniform1i(m_location, (int)data);
	}
	template<>
	void glUniform::Set<int>(int data)
	{
		glUniform1i(m_location, data);
	}
	template<>
	void glUniform::Set<unsigned int>(unsigned int data)
	{
		glUniform1ui(m_location, data);
	}
	template<>
	void glUniform::Set<float>(float data)
	{
		glUniform1f(m_location, data);
	}
	template<>
	void glUniform::Set<double>(double data)
	{
		glUniform1d(m_location, data);
	}
	template<>
	void glUniform::Set<Vector2&>(Vector2& data)
	{
		glUniform2f(m_location, data.x, data.y);
	}
	template<>
	void glUniform::Set<Vector2>(Vector2 data)
	{
		glUniform2f(m_location, data.x, data.y);
	}
	template<>
	void glUniform::Set<Vector3&>(Vector3& data)
	{
		glUniform3f(m_location, data.x, data.y, data.z);
	}
	template<>
	void glUniform::Set<Vector3>(Vector3 data)
	{
		glUniform3f(m_location, data.x, data.y, data.z);
	}
	template<>
	void glUniform::Set<Vector4&>(Vector4& data)
	{
		glUniform4f(m_location, data.x, data.y, data.z, data.w);
	}
	template<>
	void glUniform::Set<Vector4>(Vector4 data)
	{
		glUniform4f(m_location, data.x, data.y, data.z, data.w);
	}
	template<>
	void glUniform::Set<Color3F&>(Color3F& data)
	{
		glUniform3f(m_location, data.r, data.g, data.b);
	}
	template<>
	void glUniform::Set<Color3F>(Color3F data)
	{
		glUniform3f(m_location, data.r, data.g, data.b);
	}
	template<>
	void glUniform::Set<Color4F&>(Color4F& data)
	{
		glUniform4f(m_location, data.r, data.g, data.b, data.a);
	}
	template<>
	void glUniform::Set<Color4F>(Color4F data)
	{
		glUniform4f(m_location, data.r, data.g, data.b, data.a);
	}
	template<>
	void glUniform::Set<Matrix2x2&>(Matrix2x2& data)
	{
		glUniformMatrix2fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}
	template<>
	void glUniform::Set<Matrix2x2>(Matrix2x2 data)
	{
		glUniformMatrix2fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}
	template<>
	void glUniform::Set<Matrix3x3&>(Matrix3x3& data)
	{
		glUniformMatrix3fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}
	template<>
	void glUniform::Set<Matrix3x3>(Matrix3x3 data)
	{
		glUniformMatrix3fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}
	template<>
	void glUniform::Set<Matrix4x4&>(Matrix4x4& data)
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}
	template<>
	void glUniform::Set<Matrix4x4>(Matrix4x4 data)
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, data.GetStartPointer());
	}

	//	glUniformBlock::glUniformBlock(const ShaderProgram& program, const std::string& blockName, GLuint bindingPoint, GLuint bufferSize)
	//		: m_bindingPoint(bindingPoint), m_bufferSize(bufferSize)
	//	{
	//		if (m_bindingPoint >= (GLuint)glUtil::GetMaxUniformBufferBindings())
	//		{
	//			assert(FALSE);
	//			return;
	//		}
	//	
	//		m_array = new GLfloat[bufferSize / 4];
	//	
	//		m_blockIndex = glGetUniformBlockIndex(program.GetID(), blockName.c_str());
	//		glUniformBlockBinding(program.GetID(), m_blockIndex, m_bindingPoint);
	//		//
	//		glGenBuffers(1, &m_buffer);
	//		glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
	//		//
	//		// GL_STATIC_DRAW (modified few times)
	//		// GL_DYNAMIC_DRAW (modified many times)
	//		glBufferData(GL_UNIFORM_BUFFER, bufferSize, m_array, GL_DYNAMIC_DRAW);
	//		glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, m_buffer);
	//	}
	//	glUniformBlock::~glUniformBlock()
	//	{
	//		glDeleteBuffers(1, &m_buffer);
	//	
	//		if (m_array != nullptr)
	//			delete[] m_array;
	//	}
	//	
	//	void glUniformBlock::Set(GLfloat* arr, GLuint arrLength, size_t startOffset)
	//	{
	//		glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
	//		//
	//		glBufferSubData(GL_UNIFORM_BUFFER, startOffset * sizeof(GLfloat), arrLength * sizeof(GLfloat), arr);
	//		//
	//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//	}

}