// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LineMesh.h"

#include "../utilities/Asset.h"

namespace Vxl
{
	LineMesh::LineMesh(bool isVec3)
	{
		m_buffer.setBindMode(BufferUsage::DYNAMIC_DRAW);

		if (isVec3)
		{
			BufferLayout layout =
			{
				{BufferType::POSITION, AttributeType::VEC3, false},
				{BufferType::COLOR, AttributeType::VEC4, false},
				{BufferType::LINEWIDTH, AttributeType::FLOAT, false}
			};
			m_buffer.setLayout(layout);
		}
		else
		{
			BufferLayout layout =
			{
				{BufferType::POSITION, AttributeType::VEC2, false},
				{BufferType::COLOR, AttributeType::VEC4, false},
				{BufferType::LINEWIDTH, AttributeType::FLOAT, false}
			};
			m_buffer.setLayout(layout);
		}
	}

	void LineMesh::bind(DrawType type)
	{
		m_type = type;
		m_drawCount = m_buffer.GetDrawCount();

		m_VAO.bind();

		m_buffer.bind();

		m_VAO.unbind();
	}

	void LineMesh::Draw()
	{
		m_VAO.bind();
		Graphics::Draw::Array(m_type, m_drawCount);
	}
}