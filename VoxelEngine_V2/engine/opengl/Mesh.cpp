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
	void Mesh::DrawArrayInstances()
	{
		glDrawArraysInstanced((GLenum)m_type, 0, m_drawCount, m_instances.getLength());
	}
	void Mesh::DrawIndexedInstances()
	{
		glDrawElementsInstanced((GLenum)m_type, m_drawCount, GL_UNSIGNED_INT, 0, m_instances.getLength());
	}

	void Mesh::UpdateDrawInfo()
	{
		// Set Mode
		m_mode = (m_indices.getLength() == 0) ? Draw_Mode::ARRAY : Draw_Mode::INDEXED;
		if (m_instances.getLength() > 1)
			if (m_mode == Draw_Mode::ARRAY)
				m_mode = Draw_Mode::ARRAY_INSTANCED;
			else
				m_mode = Draw_Mode::INDEXED_INSTANCED;

		// Set Draw Data
		if (m_mode == Draw_Mode::ARRAY)
		{
			Draw_Function = &Mesh::DrawArray;
			m_drawCount = m_positions.getLength();
		}
		else if (m_mode == Draw_Mode::ARRAY_INSTANCED)
		{
			Draw_Function = &Mesh::DrawArrayInstances;
			m_drawCount = m_positions.getLength();
		}
		else if (m_mode == Draw_Mode::INDEXED)
		{
			Draw_Function = &Mesh::DrawIndexed;
			m_drawCount = m_indices.getLength();
		}
		else
		{
			Draw_Function = &Mesh::DrawIndexedInstances;
			m_drawCount = m_indices.getLength();
		}
	}

	Mesh::Mesh()
	{
		// Buffer Objects
		delete[] m_VBOs;
		m_VBOs = new GLuint[5];
		glUtil::generateVAO(m_VAO, m_VBOs[0], 5);

		// Buffers
		m_positions.setup	(m_VBOs[0], 0);
		m_uvs.setup			(m_VBOs[1], 1);
		m_normals.setup		(m_VBOs[2], 2);
		m_instances.setup   (m_VBOs[3], 3);
		m_indices.setup		(m_VBOs[4]);
	}
	Mesh::~Mesh()
	{
		glUtil::deleteVAO(m_VAO, m_VBOs[0], 4);
		delete[] m_VBOs;
	}

	void Mesh::Bind(Draw_Type type)
	{
		m_type = type;
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

		m_positions.bind();
		m_uvs.bind();
		m_normals.bind();
		m_instances.bind();
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