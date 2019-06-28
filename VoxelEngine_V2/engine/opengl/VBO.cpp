// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "VBO.h"

namespace Vxl
{
	// VBO //
	void VBO::UpdateDrawCount()
	{
		m_StrideSize = 0;
		for (auto& hint : m_strideHints)
		{
			m_StrideSize += hint.second.m_valueCount * 4; // Total bytes
		}

		if (m_StrideSize == 0)
			m_DrawCount = m_Size / m_TypeSize;
		else
			m_DrawCount = m_Size / m_StrideSize;
	}

	void VBO::AddStrideHint(BufferType _type, GLuint _valueCount, GLuint _strideOffset)
	{
		m_strideHints[(UINT)_type] = StrideHint(_valueCount, _strideOffset * 4);

		if (_type == BufferType::InstancingStart)
			m_strideHints[(UINT)_type].m_instancing = true;

		UpdateDrawCount();
	}

	void VBO::RemoveStrideHint(BufferType _type)
	{
		m_strideHints.erase((UINT)_type);
		
		UpdateDrawCount();
	}

	void VBO::Bind()
	{
		if (m_empty)
			return;

		glUtil::bindVBO(m_VBO);

		UINT strideCount = m_strideHints.size();

		switch (strideCount)
		{
		case 0: // Exit if no stride information
			return;

		case 1:
			for (auto hint : m_strideHints)
			{
			if (hint.second.m_instancing)
				glUtil::setVertexAttribInstancing((UINT)hint.first);
			else
				glUtil::setVertexAttrib((UINT)hint.first, hint.second.m_valueCount, Data_Type::FLOAT, 0, 0);
			}
		
		default:
			for (auto hint : m_strideHints)
			{
				if (hint.second.m_instancing)
					glUtil::setVertexAttribInstancing((UINT)hint.first);
				else
					glUtil::setVertexAttrib((UINT)hint.first, hint.second.m_valueCount, Data_Type::FLOAT, m_StrideSize, hint.second.m_strideOffset);
			}
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
