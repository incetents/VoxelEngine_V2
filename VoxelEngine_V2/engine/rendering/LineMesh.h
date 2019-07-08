// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
//#include "glUtil.h"
#include "Graphics.h"

#include "../utilities/Asset.h"

#include <vector>

namespace Vxl
{
	class LineMesh : public Asset<LineMesh>
	{
	private:
		VAO m_VAO;
		VBO	m_VBO;

		DrawType m_type;
		uint32_t m_drawCount = 0;	// Vertices Drawn


	public:
		std::vector<float>	m_vertices;
		UINT				m_vertexIndex = 0;
		bool				m_resizeDirty = false;

		LineMesh(bool isVec3)
		{
			if (isVec3)
			{
				BufferLayout layout =
				{
					{BufferType::POSITION, AttributeType::VEC3, false},
					{BufferType::COLOR, AttributeType::VEC4, false},
					{BufferType::LINEWIDTH, AttributeType::FLOAT, false}
				};
				m_VBO.SetLayout(layout);
			}
			else
			{
				BufferLayout layout =
				{
					{BufferType::POSITION, AttributeType::VEC2, false},
					{BufferType::COLOR, AttributeType::VEC4, false},
					{BufferType::LINEWIDTH, AttributeType::FLOAT, false}
				};
				m_VBO.SetLayout(layout);
			}
		}

		// Expensive but can resize buffer
		void SetVertices()
		{
			if (m_vertexIndex > 0)
				m_VBO.SetVertices(m_vertices, BufferUsage::DYNAMIC_DRAW);
		}
		// Cheaper but cannot resize buffer
		void UpdateVertices()
		{
			if(m_vertexIndex > 0)
				m_VBO.UpdateVertices(&m_vertices[0], 0);
		}

		void Bind(DrawType type = DrawType::LINES);
		void Draw();
	};
}

