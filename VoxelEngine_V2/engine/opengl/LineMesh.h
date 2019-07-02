// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
#include "glUtil.h"

#include "../utilities/Asset.h"

#include <vector>

namespace Vxl
{
	class LineMesh : public Asset<LineMesh>
	{
	private:
		VAO m_VAO;
		VBO	m_VBO;

		Draw_Type m_type;
		GLuint	  m_drawCount = 0;	// Vertices Drawn


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
					{BufferType::POSITION, ShaderDataType::VEC3, false},
					{BufferType::COLOR, ShaderDataType::VEC4, false},
					{BufferType::LINEWIDTH, ShaderDataType::FLOAT, false}
				};
				m_VBO.SetLayout(layout);
			}
			else
			{
				BufferLayout layout =
				{
					{BufferType::POSITION, ShaderDataType::VEC2, false},
					{BufferType::COLOR, ShaderDataType::VEC4, false},
					{BufferType::LINEWIDTH, ShaderDataType::FLOAT, false}
				};
				m_VBO.SetLayout(layout);
			}
		}

		//	void AddStrideHint(BufferType type, GLuint valueCount, GLuint strideByteOffset)
		//	{
		//		m_vertexBuffer.addStrideHint(type, valueCount, strideByteOffset);
		//	}
		//	void RemoveStrideHint(BufferType type)
		//	{
		//		m_vertexBuffer.removeStrideHint(type);
		//	}
		//	void ClearStrideHints(void)
		//	{
		//		m_vertexBuffer.removeAllStrideHints();
		//	}

		// Expensive but can resize buffer
		void SetVertices()
		{
			if (m_vertexIndex > 0)
				m_VBO.SetVertices(m_vertices, BufferBind_Mode::DYNAMIC);
		}
		// Cheaper but cannot resize buffer
		void UpdateVertices()
		{
			if(m_vertexIndex > 0)
				m_VBO.UpdateVertices(&m_vertices[0], 0);
		}

		void Bind(Draw_Type type = Draw_Type::LINES);
		void Draw();
	};
}

