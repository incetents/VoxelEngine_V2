// Copyright (c) 2019 Emmanuel Lajeunesse
// Referenced from: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/Buffer.h
#pragma once

#include "Graphics.h"

#include <vector>

namespace Vxl
{
	enum class AttributeLocation
	{
		LOC0 = 0, LOC1, LOC2, LOC3, LOC4, LOC5, LOC6, LOC7, LOC8, LOC9,
		LOC10, LOC11, LOC12, LOC13, LOC14, LOC15, LOC16, LOC17, LOC18, LOC19,
		LOC20, LOC21, LOC22, LOC23, LOC24, LOC25, LOC26, LOC27, LOC28, LOC29,
		LOC30, LOC31, LOC32, LOC33, LOC34, LOC35, LOC36, LOC37, LOC38, LOC39,
		LOC40, LOC41, LOC42, LOC43, LOC44, LOC45, LOC46, LOC47, LOC48, LOC49,
		LOC50, LOC51, LOC52, LOC53, LOC54, LOC55, LOC56, LOC57, LOC58, LOC59,
		LOC60, LOC61, LOC62, LOC63, LOC64, LOC65, LOC66, LOC67, LOC68, LOC69,
		LOC70, LOC71, LOC72, LOC73, LOC74, LOC75, LOC76, LOC77, LOC78, LOC79,
		LOC80, LOC81, LOC82, LOC83, LOC84, LOC85, LOC86, LOC87, LOC88, LOC89,
		LOC90, LOC91, LOC92, LOC93, LOC94, LOC95, LOC96, LOC97, LOC98, LOC99,
	};

	// List of buffer types (non-custom names reflect built-in attribute locations)
	//	enum class BufferType
	//	{
	//		NONE = -1, // Dev use only
	//		POSITION = 0, VERTEX = 0,
	//		UV = 1, LINEWIDTH = 1,
	//		NORMAL = 2,
	//		COLOR = 3,
	//		SECOND_COLOR = 4,
	//		FOG = 5,
	//		TANGENT = 6,
	//		UV1 = 8,  INSTANCING_1 = 8,
	//		UV2 = 9,  INSTANCING_2 = 9,
	//		UV3 = 10, INSTANCING_3 = 10,
	//		UV4 = 11, INSTANCING_4 = 11,
	//		UV5 = 12,
	//		UV6 = 13,
	//		UV7 = 14,
	//		UV8 = 15
	//	};

	// Vertex Buffer Element
	class BufferElement
	{
		friend class BufferLayout;
		friend class VBO;
	private:
		AttributeLocation	m_attributeLocation;
		DataType			m_dataType;	
		AttributeType		m_shaderDataType;
		bool				m_normalized;
		uint32_t			m_valueCount;	// Values [Ex: float = 1, double = 1, vec4 = 4]
		uint32_t			m_size;			// Bytes  [Ex: float = 4, double = 8, vec4 = 16]
		uint32_t			m_offset = 0;   // Bytes between each segment [ex: layout="xyzrgba" -> requires 12 bits of offset to reach start of rgba]
		uint32_t			m_divisor = 0;

	public:
		BufferElement(AttributeLocation attribLocation, AttributeType shaderDataType, bool normalized = false, uint32_t divisor = 0);
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
		BufferLayout(const std::initializer_list<BufferElement>& elements);
	};
}