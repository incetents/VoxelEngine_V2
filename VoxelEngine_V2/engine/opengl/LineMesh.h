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

		Draw_Type m_type;
		GLuint	  m_drawCount = 0;	// Vertices Drawn

		MeshBuffer<float>	m_vertexBuffer;

	public:
		std::vector<float>	m_vertices;
		UINT				m_vertexIndex = 0;
		bool				m_resizeDirty = false;

		void AddStrideHint(BufferType type, int valueCount)
		{
			m_vertexBuffer.addStrideHint(type, valueCount);
		}
		// Expensive but can resize buffer
		void SetVertices()
		{
			if (m_vertexIndex > 0)
				m_vertexBuffer.set(m_vertices, BufferBind_Mode::DYNAMIC);
		}
		// Cheaper but cannot resize buffer
		void UpdateVertices()
		{
			if(m_vertexIndex > 0)
				m_vertexBuffer.update(m_vertices);
		}

		void Bind(Draw_Type type = Draw_Type::LINES);
		void Draw();
	};
}

