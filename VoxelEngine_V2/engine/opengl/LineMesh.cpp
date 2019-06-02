// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LineMesh.h"

namespace Vxl
{
	void LineMesh::Bind(Draw_Type type)
	{
		m_type = type;
		m_drawCount = m_vertexBuffer.GetDrawCount();

		m_VAO.bind();

		m_vertexBuffer.m_vbo.Bind();

		m_VAO.unbind();
	}

	void LineMesh::Draw()
	{
		m_VAO.bind();
		glDrawArrays((GLenum)m_type, 0, m_drawCount);
	}
}