// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "VBO.h"

namespace Vxl
{
	// VBO //
	void VBO::UpdateDrawCount()
	{
		m_DrawCount = m_Size / m_layout.m_stride;
	}

	void VBO::Bind()
	{
		if (m_empty)
			return;

		glUtil::bindVBO(m_VBO);

		uint32_t elementCount = (uint32_t)m_layout.m_elements.size();
		VXL_ASSERT(elementCount > 0, "Layout requires at least one Element");

		for (const auto& element : m_layout.m_elements)
		{
			glUtil::setVertexAttrib((UINT)element.m_bufferType, element.m_valueCount, Data_Type::FLOAT, m_layout.m_stride, element.m_offset, element.m_normalized);

			if (element.m_divisor > 0)
				glUtil::setVertexAttribDivisor((UINT)element.m_bufferType, element.m_divisor);
		}
	}

	void VBO::Draw(Draw_Type _draw)
	{
		glDrawArrays((GLenum)_draw, 0, m_DrawCount);
	}

	// EBO //

	void EBO::SetIndices(GLuint* _arr, GLuint _count, BufferBind_Mode _mode)
	{
		if (_count == 0)
			return;

		m_empty = false;

		if (m_EBO == -1)
			glGenBuffers(1, &m_EBO);

		m_Size = _count * sizeof(GLuint);
		m_DrawCount = _count;
		m_bindMode = _mode;
		glUtil::bindIndices(m_EBO, m_Size, _arr, (GLenum)_mode);
	}
	void EBO::SetIndices(std::vector<GLuint> _arr, BufferBind_Mode _mode)
	{
		SetIndices(&_arr[0], (GLuint)_arr.size(), _mode);
	}
	void EBO::Bind()
	{
		if (m_EBO != -1)
			glUtil::bindEBO(m_EBO);
	}
	void EBO::Draw(Draw_Type _draw)
	{
		glDrawElements((GLenum)_draw, m_DrawCount, GL_UNSIGNED_INT, 0);
	}
}

// Legacy Reference //
//	glEnableVertexAttribArray((int)BufferType::POSITION);
//	glEnableVertexAttribArray((int)BufferType::COLOR);
//	
//	glVertexAttribPointer((int)BufferType::POSITION, 3, (GLenum)DataType::FLOAT, GL_FALSE, m_StrideSize, BUFFER_OFFSET(0));
//	glVertexAttribPointer((int)BufferType::COLOR, 3, (GLenum)DataType::FLOAT, GL_FALSE, m_StrideSize, BUFFER_OFFSET(3 * sizeof(GLfloat)));
