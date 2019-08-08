// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LineMesh.h"

namespace Vxl
{
	void LineMesh::Bind(DrawType type)
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