// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Mesh.h"

#include <assert.h>
#include <iostream>

namespace Vxl
{
	Database<Mesh> Mesh::m_database;

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
		glDrawArraysInstanced((GLenum)m_type, 0, m_drawCount, m_instances.GetDrawCount());
	}
	void Mesh::DrawIndexedInstances()
	{
		glDrawElementsInstanced((GLenum)m_type, m_drawCount, GL_UNSIGNED_INT, 0, m_instances.GetDrawCount());
	}

	void Mesh::UpdateDrawInfo()
	{
		// Set Mode
		m_mode = (m_indices.GetDrawCount() == 0) ? Draw_Mode::ARRAY : Draw_Mode::INDEXED;
		if (m_instances.GetDrawCount() > 1)
			if (m_mode == Draw_Mode::ARRAY)
				m_mode = Draw_Mode::ARRAY_INSTANCED;
			else
				m_mode = Draw_Mode::INDEXED_INSTANCED;

		// Set Draw Data
		if (m_mode == Draw_Mode::ARRAY)
		{
			Draw_Function = &Mesh::DrawArray;
			m_drawCount = m_positions.GetDrawCount();
		}
		else if (m_mode == Draw_Mode::ARRAY_INSTANCED)
		{
			Draw_Function = &Mesh::DrawArrayInstances;
			m_drawCount = m_positions.GetDrawCount();
			
		}
		else if (m_mode == Draw_Mode::INDEXED)
		{
			Draw_Function = &Mesh::DrawIndexed;
			m_drawCount = m_indices.GetDrawCount();
		}
		else
		{
			Draw_Function = &Mesh::DrawIndexedInstances;
			m_drawCount = m_indices.GetDrawCount();
		}

		// Update Special Data
		switch (m_type)
		{
		case Draw_Type::NO_DRAW:
			m_faces = 0;
			m_lines = 0;
			break;
		case Draw_Type::POINTS:
			m_faces = 0;
			m_lines = 0;
			break;
		case Draw_Type::LINES:
			assert(m_drawCount % 2 == 0);
			m_faces = 0;
			m_lines = m_drawCount / 2;
			break;
		case Draw_Type::LINE_STRIP:
			m_faces = 0;
			m_lines = m_drawCount - 1;
			break;
		case Draw_Type::LINE_LOOP:
			m_faces = 0;
			m_lines = m_drawCount;
			break;
		case Draw_Type::LINES_ADJACENCY:
			assert(m_drawCount % 4 == 0);
			m_faces = 0;
			m_lines = m_drawCount / 4;
			break;
		case Draw_Type::LINE_STRIP_ADJACENCY:
			assert(m_drawCount > 1);
			m_faces = 0;
			m_lines = m_drawCount - 2;
			break;

		case Draw_Type::TRIANGLES:
			assert(m_drawCount % 3 == 0);
			m_faces = m_drawCount / 3;
			m_lines = m_drawCount;
			break;
		case Draw_Type::TRIANGLE_STRIP:
		case Draw_Type::TRIANGLE_FAN:
			assert(m_drawCount > 2);
			m_faces = m_drawCount - 2;
			m_lines = ((m_drawCount - 3) * 2) + 3;
			break;
		case Draw_Type::TRIANGLES_ADJACENCY:
			assert(m_drawCount % 6 == 0);
			m_faces = m_drawCount / 6;
			m_lines = m_drawCount / 3;
			break;
		

		case Draw_Type::TRIANGLE_STRIP_ADJACENCY:
			// Not understood
			m_faces = 0;
			m_lines = 0;
			assert(false);
			break;

		case Draw_Type::PATCHES:
			// Not implemented
			m_faces = 0;
			m_lines = 0;
			assert(false);
			break;
			
		}
		
	}

	Mesh::Mesh()
	{
		m_VAO = glUtil::generateVAO();
	}

	Mesh* Mesh::Create(const std::string& name)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Mesh: " + name);
			return nullptr;
		}

		Mesh* M = new Mesh();

		Logger.log("Created Mesh: " + name);
		return M;
	}

	Mesh::~Mesh()
	{
		glUtil::deleteVAO(m_VAO);
	}

	void Mesh::GenerateNormals(
		Vector3* _vertices, GLuint _vertCount,
		GLuint* _indices, GLuint _indexCount
	){
		std::vector<Vector3> Normals;
		Normals.resize(_vertCount);
		//
		if (_vertices == nullptr || _vertCount == 0)
		{
			assert(false);
			return;
		}
		// Indexed Normals
		if (_indices)
		{
			assert(_indexCount % 3 == 0 && _indexCount > 0);
			
			for (GLuint i = 0; i < _indexCount; i+= 3)
			{
				Vector3 P1(
					_vertices[_indices[i + 0]].x,
					_vertices[_indices[i + 0]].y,
					_vertices[_indices[i + 0]].z
				);
				Vector3 P2(
					_vertices[_indices[i + 1]].x,
					_vertices[_indices[i + 1]].y,
					_vertices[_indices[i + 1]].z
				);
				Vector3 P3(
					_vertices[_indices[i + 2]].x,
					_vertices[_indices[i + 2]].y,
					_vertices[_indices[i + 2]].z
				);
				Vector3 N = Vector3::Cross((P2 - P1), (P3 - P1));
				Normals[_indices[i + 0]] += N;
				Normals[_indices[i + 1]] += N;
				Normals[_indices[i + 2]] += N;
			}
		}
		// Position Normals
		else
		{
			assert(_vertCount % 3 == 0);

			for (GLuint i = 0; i < _vertCount; i += 3)
			{
				Vector3 P1(
					_vertices[i + 0].x,
					_vertices[i + 0].y,
					_vertices[i + 0].z
				);
				Vector3 P2(
					_vertices[i + 1].x,
					_vertices[i + 1].y,
					_vertices[i + 1].z
				);
				Vector3 P3(
					_vertices[i + 2].x,
					_vertices[i + 2].y,
					_vertices[i + 2].z
				);
				Vector3 N = Vector3::Cross((P2 - P1), (P3 - P1));
				Normals[i + 0] += N;
				Normals[i + 1] += N;
				Normals[i + 2] += N;
			}
		}
		// Normalize the normals
		for (GLuint i = 0; i < _vertCount; i++)
		{
			Normals[i].NormalizeSelf();
		}
		//
		m_normals.set(Normals);
	}

	void Mesh::Bind(Draw_Type type)
	{
		m_type = type;
		UpdateDrawInfo();

		// SIZE CHECK //
#ifdef _DEBUG
		GLuint indicesCount = m_indices.GetDrawCount();
		if (indicesCount)
		{
			if (glUtil::isDrawTypeTriangles(m_type))
				assert(indicesCount % 3 == 0);
			else if (glUtil::isDrawTypeLines(m_type))
				assert(indicesCount % 2 == 0);
		}
		else
		{
			GLuint posCount = m_positions.GetDrawCount();
			if (glUtil::isDrawTypeTriangles(m_type))
				assert(posCount % 3 == 0);
			else if (glUtil::isDrawTypeLines(m_type))
				assert(posCount % 2 == 0);
		}
#endif

		/*	Bind Data	*/
		glUtil::bindVAO(m_VAO);

		m_positions.m_vbo.Bind();
		m_uvs.m_vbo.Bind();
		m_normals.m_vbo.Bind();
		m_tangents.m_vbo.Bind();
		m_bitangents.m_vbo.Bind();
		m_colors.m_vbo.Bind();
		m_instances.m_vbo.Bind();
		m_indices.m_vboi.Bind();

		glUtil::unbindVAO();
		/*				*/	

		// Min/Max
		m_min = Vector3::ZERO;
		m_max = Vector3::ZERO;
		UINT PosCount = (UINT)m_positions.vertices.size();
		for (UINT i = 0; i < PosCount; i++)
		{
			m_min = Vector3::Min(m_min, m_positions.vertices[i]);
			m_max = Vector3::Max(m_max, m_positions.vertices[i]);
		}
	}

	void Mesh::Draw()
	{
		glUtil::bindVAO(m_VAO);
		(*this.*Draw_Function)();
	}
}