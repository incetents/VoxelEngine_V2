// Copyright (c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "VBOLayout.h"

#include "Graphics.h"

namespace Vxl
{
	BufferElement::BufferElement(AttributeLocation attribLocation, AttributeType shaderDataType, bool normalized, uint32_t divisor)
		: m_attributeLocation(attribLocation), m_shaderDataType(shaderDataType), m_normalized(normalized), m_divisor(divisor)
	{
		VXL_ASSERT((int)m_attributeLocation <= Graphics::GLMaxAttributes,
			"Attribute location is too large; Current: " + std::to_string((int)attribLocation) + ", Max: " + std::to_string(Graphics::GLMaxAttributes)
		);

		m_valueCount = Graphics::GetValueCount(shaderDataType);
		m_size = Graphics::GetSize(shaderDataType);
		m_dataType = Graphics::GetDataType(shaderDataType);
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_elements(elements)
	{
		m_stride = 0;
		uint32_t offset = 0;
		for (auto& element : m_elements)
		{
			element.m_offset = offset;
			offset += element.m_size;
			m_stride += element.m_size;
		}
	}
}