// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "VBO.h"

namespace Vxl
{
	// VBO //
	void VBO::UpdateDrawCount()
	{
		if (m_Stride == 0)
			m_DrawCount = m_Size / m_TypeSize;
		else
			m_DrawCount = m_Size / m_Stride;

		UINT Offset = 0;
		UINT Count = (UINT)m_strideHints.size();
		for (UINT i = 0; i < Count; i++)
		{
			m_strideHints[i].m_strideOffset = Offset;
			Offset += m_strideHints[i].m_valueCount * sizeof(GLfloat);
		}
	}

	void VBO::AddStrideHint(BufferType _type, GLuint _valueCount)
	{
		StrideHint* hint = GetHint(_type);

		// Check if hint already exists
		if (hint)
		{
			m_Stride -= hint->m_valueCount * m_TypeSize;
			hint->m_valueCount = _valueCount;
		}
		// New
		else
		{
			StrideHint hint;
			hint.m_type = _type;
			hint.m_valueCount = _valueCount;
			m_strideHints.push_back(hint);
			m_strideCount++;
		}

		// Update Stride
		m_Stride += _valueCount * m_TypeSize;

		// Update Data
		UpdateDrawCount();
	}

	void VBO::RemoveStrideHint(BufferType _type)
	{
		StrideHint* hint = GetHint(_type);

		// Check if hint already exists
		if (hint)
		{
			m_Stride -= hint->m_valueCount * sizeof(GLfloat);
			RemoveHint(_type);
			delete hint;
			UpdateDrawCount();
			m_strideCount--;
		}
	}

	void VBO::Bind()
	{
		if (m_empty)
			return;

		glUtil::bindVBO(m_VBO);

		switch (m_strideCount)
		{
		case 1:
			if (m_strideHints[0].m_type == BufferType::InstancingStart)
				glUtil::setVertexAttribInstancing((int)m_strideHints[0].m_type);
			else
				glUtil::setVertexAttrib((int)m_strideHints[0].m_type, m_strideHints[0].m_valueCount, DataType::FLOAT, 0, 0);
		case 0: // Exit if no stride information
			return;
		default:
			for (auto hint : m_strideHints)
			{
				if (hint.m_type == BufferType::InstancingStart)
					glUtil::setVertexAttribInstancing((int)hint.m_type);
				else
					glUtil::setVertexAttrib((int)hint.m_type, hint.m_valueCount, DataType::FLOAT, m_Stride, hint.m_strideOffset);
			}
		}
	}

	void VBO::Draw(Draw_Type _draw)
	{
		glDrawArrays((GLenum)_draw, 0, m_DrawCount);
	}

	// VBOI //

	void VBOI::SetIndices(GLuint* _arr, GLuint _count, BufferBind_Mode _mode)
	{
		if (_count == 0)
			return;

		m_empty = false;

		if (m_VBOI == -1)
			glGenBuffers(1, &m_VBOI);

		m_Size = _count * sizeof(GLuint);
		m_DrawCount = _count;
		m_bindMode = _mode;
		glUtil::bindIndices(m_VBOI, m_Size, _arr, (GLenum)_mode);
	}
	void VBOI::SetIndices(std::vector<GLuint> _arr, BufferBind_Mode _mode)
	{
		SetIndices(&_arr[0], (GLuint)_arr.size(), _mode);
	}
	void VBOI::Bind()
	{
		glUtil::bindVBOI(m_VBOI);
	}
	void VBOI::Draw(Draw_Type _draw)
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
