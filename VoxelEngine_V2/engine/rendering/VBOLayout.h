// Copyright (c) 2019 Emmanuel Lajeunesse
// Referenced from: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/Buffer.h
#pragma once

#include "Graphics.h"

#include <vector>

namespace Vxl
{
	// List of buffer types (non-custom names reflect built-in attribute locations)
	enum class BufferType
	{
		NONE = -1, // Dev use only
		POSITION = 0, VERTEX = 0,
		UV = 1, LINEWIDTH = 1,
		NORMAL = 2,
		COLOR = 3,
		SECOND_COLOR = 4,
		FOG = 5,
		TANGENT = 6,
		UV1 = 8,  INSTANCING_1 = 8,
		UV2 = 9,  INSTANCING_2 = 9,
		UV3 = 10, INSTANCING_3 = 10,
		UV4 = 11, INSTANCING_4 = 11,
		UV5 = 12,
		UV6 = 13,
		UV7 = 14,
		UV8 = 15
	};

	// Vertex Buffer Element
	class BufferElement
	{
		friend class BufferLayout;
		friend class VBO;
	private:
		BufferType		m_bufferType;	// For vertex attrib slot
		DataType		m_dataType;	
		AttributeType	m_shaderDataType;
		bool			m_normalized;
		uint32_t		m_valueCount;	// Values [Ex: float = 1, double = 1, vec4 = 4]
		uint32_t		m_size;			// Bytes  [Ex: float = 4, double = 8, vec4 = 16]
		uint32_t		m_offset = 0;   // Bytes between each segment [ex: layout="xyzrgba" -> requires 12 bits of offset to reach start of rgba]
		uint32_t		m_divisor = 0;

	public:
		BufferElement(BufferType bufferType, AttributeType shaderDataType, bool normalized = false, uint32_t divisor = 0)
			: m_bufferType(bufferType), m_shaderDataType(shaderDataType), m_normalized(normalized), m_divisor(divisor)
		{
			m_valueCount	= Graphics::GetValueCount(shaderDataType);
			m_size			= Graphics::GetSize(shaderDataType);
			m_dataType		= Graphics::GetDataType(shaderDataType);
		}
	};
	// Vertex Buffer Layout
	class BufferLayout
	{
		friend class VBO;
	private:
		int test;
		std::vector<BufferElement>	m_elements;
		uint32_t					m_stride; // Bytes

	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		void CalculateOffsetsAndStride()
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
	};
}