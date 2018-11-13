// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Mesh.h"

#include <assert.h>
#include <iostream>

namespace Vxl
{
	void Mesh::DrawArray()
	{
		glDrawArrays((GLenum)m_type, 0, m_drawCount);
	}
	void Mesh::DrawIndexed()
	{
		glDrawElements((GLenum)m_type, m_drawCount, GL_UNSIGNED_INT, 0);
	}

	void Mesh::UpdateDrawInfo()
	{
		m_mode = (m_indices.getLength() == 0) ? Draw_Mode::ARRAY : Draw_Mode::INDEXED;

		if (m_mode == Draw_Mode::ARRAY)
		{
			Draw_Function = &Mesh::DrawArray;
			m_drawCount = m_positions.getLength();
		}
		else
		{
			Draw_Function = &Mesh::DrawIndexed;
			m_drawCount = m_indices.getLength();
		}
	}

	Mesh::Mesh()
	{
		// Buffer Objects
		delete[] m_VBOs;
		m_VBOs = new GLuint[4];
		glUtil::generateVAO(m_VAO, m_VBOs[0], 4);

		// Buffers
		m_positions.setup	(m_VBOs[0], 0);
		m_uvs.setup			(m_VBOs[1], 1);
		m_normals.setup		(m_VBOs[2], 2);
		m_indices.setup		(m_VBOs[3]);
	}
	Mesh::~Mesh()
	{
		glUtil::deleteVAO(m_VAO, m_VBOs[0], 4);
		delete[] m_VBOs;
	}

	void Mesh::Bind()
	{
		UpdateDrawInfo();

		// SIZE CHECK //
#ifdef _DEBUG
		GLuint indicesCount = m_indices.getLength();
		if (indicesCount)
		{
			if (glUtil::isDrawTypeTriangles(m_type))
				assert(indicesCount % 3 == 0);
			else if (glUtil::isDrawTypeLines(m_type))
				assert(indicesCount % 2 == 0);
		}
		else
		{
			GLuint posCount = m_positions.getLength();
			if (glUtil::isDrawTypeTriangles(m_type))
				assert(posCount % 3 == 0);
			else if (glUtil::isDrawTypeLines(m_type))
				assert(posCount % 2 == 0);
		}
#endif

		/*	Bind Data	*/
		glUtil::bindVAO(m_VAO);

		if(m_positions.getLength())
			m_positions.bind();

		if (m_uvs.getLength())
			m_uvs.bind();

		if (m_normals.getLength())
			m_normals.bind();

		if (m_indices.getLength())
			m_indices.bind();

		glUtil::unbindVAO();
		/*				*/	
	}

	void Mesh::Draw()
	{
		glUtil::bindVAO(m_VAO);
		(*this.*Draw_Function)();
	}
}