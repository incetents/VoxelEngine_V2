// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "VBO.h"

namespace Vxl
{
	// VBO //
	void VBO::UpdateDrawCount()
	{
		m_DrawCount = m_Size / m_layout.m_stride;
	}

	void VBO::bind() const
	{
		if (m_empty)
			return;

		Graphics::VBO::bind(m_VBO);

		uint32_t elementCount = (uint32_t)m_layout.m_elements.size();
		VXL_ASSERT(elementCount > 0, "Layout requires at least one Element");

		// If only 1 element, stride and offset can be set to zero for efficient packing
		if (elementCount == 1)
		{
			const BufferElement& element = m_layout.m_elements[0];

			Graphics::VBO::SetVertexAttribState((uint32_t)element.m_attributeLocation, true);
			Graphics::VBO::SetVertexAttrib((uint32_t)element.m_attributeLocation, element.m_valueCount, element.m_dataType, 0, 0, element.m_normalized);

			if (element.m_divisor > 0)
				Graphics::VBO::SetVertexAttribDivisor((uint32_t)element.m_attributeLocation, element.m_divisor);
		}
		// Normal layout
		else
		{
			for (const auto& element : m_layout.m_elements)
			{
				Graphics::VBO::SetVertexAttribState((uint32_t)element.m_attributeLocation, true);
				Graphics::VBO::SetVertexAttrib((uint32_t)element.m_attributeLocation, element.m_valueCount, element.m_dataType, m_layout.m_stride, element.m_offset, element.m_normalized);

				if (element.m_divisor > 0)
					Graphics::VBO::SetVertexAttribDivisor((uint32_t)element.m_attributeLocation, element.m_divisor);
			}
		}
	}

	// EBO //

	void EBO::SetIndices(uint32_t* _arr, uint32_t _count, BufferUsage _mode)
	{
		VXL_ASSERT(_mode != BufferUsage::NONE, "Incorrect BufferUsage for EBO");
		m_bindMode = _mode;

		if (_count == 0)
			return;

		m_empty = false;

		if (m_EBO == -1)
			m_EBO = Graphics::EBO::Create();

		m_Size = _count * sizeof(uint32_t);
		m_DrawCount = _count;
		Graphics::EBO::bind(m_EBO);
		Graphics::EBO::BindData(m_Size, _arr, _mode);
	}
	void EBO::SetIndices(std::vector<uint32_t> _arr, BufferUsage _mode)
	{
		SetIndices(&_arr[0], (uint32_t)_arr.size(), _mode);
	}
	void EBO::bind() const
	{
		if (m_EBO != -1)
			Graphics::EBO::bind(m_EBO);
	}
}

// Legacy Reference //
//	glEnableVertexAttribArray((int)BufferType::POSITION);
//	glEnableVertexAttribArray((int)BufferType::COLOR);
//	
//	glVertexAttribPointer((int)BufferType::POSITION, 3, (GLenum)DataType::FLOAT, GL_FALSE, m_StrideSize, BUFFER_OFFSET(0));
//	glVertexAttribPointer((int)BufferType::COLOR, 3, (GLenum)DataType::FLOAT, GL_FALSE, m_StrideSize, BUFFER_OFFSET(3 * sizeof(GLfloat)));
