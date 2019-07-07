// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Mesh.h"

#include "Graphics.h"

#include "../math/Model.h"

#include "../utilities/logger.h"
#include "../utilities/Macros.h"

#include <assert.h>
#include <iostream>

namespace Vxl
{
	void Mesh::DrawArray()
	{
		Graphics::Draw::Array(m_type, m_drawCount);
	}
	void Mesh::DrawIndexed()
	{
		Graphics::Draw::Indexed(m_type, m_drawCount);
	}
	void Mesh::DrawArrayInstances()
	{
		Graphics::Draw::ArrayInstanced(m_type, m_drawCount, m_instances.GetDrawCount());
	}
	void Mesh::DrawIndexedInstances()
	{
		Graphics::Draw::IndexedInstanced(m_type, m_drawCount, m_instances.GetDrawCount());
	}

	void Mesh::UpdateDrawInfo()
	{
		// Set Mode
		m_mode = (m_indices.GetDrawCount() == 0) ? DrawMode::ARRAY : DrawMode::INDEXED;
		if (m_instances.GetDrawCount() > 1)
			if (m_mode == DrawMode::ARRAY)
				m_mode = DrawMode::ARRAY_INSTANCED;
			else
				m_mode = DrawMode::INDEXED_INSTANCED;

		// Set Draw Data
		if (m_mode == DrawMode::ARRAY)
		{
			Draw_Function = &Mesh::DrawArray;
			m_drawCount = m_positions.GetDrawCount();
		}
		else if (m_mode == DrawMode::ARRAY_INSTANCED)
		{
			Draw_Function = &Mesh::DrawArrayInstances;
			m_drawCount = m_positions.GetDrawCount();
			
		}
		else if (m_mode == DrawMode::INDEXED)
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
		case DrawType::NONE:
			m_faces = 0;
			m_lines = 0;
			break;
		case DrawType::POINTS:
			m_faces = 0;
			m_lines = 0;
			break;
		case DrawType::LINES:
			VXL_ASSERT(m_drawCount % 2 == 0, "Mesh DrawCount is not multiple of 2 for LINES");
			m_faces = 0;
			m_lines = m_drawCount / 2;
			break;
		case DrawType::LINE_STRIP:
			m_faces = 0;
			m_lines = m_drawCount - 1;
			break;
		case DrawType::LINE_LOOP:
			m_faces = 0;
			m_lines = m_drawCount;
			break;
		case DrawType::LINES_ADJACENCY:
			VXL_ASSERT(m_drawCount % 4 == 0, "Mesh DrawCount is not multiple of 4 for LINES_ADJACENCY");
			m_faces = 0;
			m_lines = m_drawCount / 4;
			break;
		case DrawType::LINE_STRIP_ADJACENCY:
			VXL_ASSERT(m_drawCount > 1, "Mesh DrawCount is not larger than 1 for LINE_STRIP_ADJACENCY");
			m_faces = 0;
			m_lines = m_drawCount - 2;
			break;

		case DrawType::TRIANGLES:
			VXL_ASSERT(m_drawCount % 3 == 0, "Mesh DrawCount is not multiple of 3 for TRIANGLES");
			m_faces = m_drawCount / 3;
			m_lines = m_drawCount;
			break;
		case DrawType::TRIANGLE_STRIP:
		case DrawType::TRIANGLE_FAN:
			VXL_ASSERT(m_drawCount > 2, "Mesh DrawCount is not larger than 2 for TRIANGLE_STRIP/TRIANGLE_FAN");
			m_faces = m_drawCount - 2;
			m_lines = ((m_drawCount - 3) * 2) + 3;
			break;
		case DrawType::TRIANGLES_ADJACENCY:
			VXL_ASSERT(m_drawCount % 6 == 0, "Mesh DrawCount is not multiple of 6 for TRIANGLES_ADJACENCY");
			m_faces = m_drawCount / 6;
			m_lines = m_drawCount / 3;
			break;
		

		case DrawType::TRIANGLE_STRIP_ADJACENCY:
			// Not understood
			m_faces = 0;
			m_lines = 0;
			VXL_ASSERT(false, "DrawType Not implemented properly");
			break;

		case DrawType::PATCHES:
			// Not implemented
			m_faces = 0;
			m_lines = 0;
			VXL_ASSERT(false, "DrawType Not implemented properly");
			break;
			
		}
		
	}

	Mesh::Mesh(const std::string& glName)
	{
		if (!glName.empty())
			Graphics::SetGLName(ObjectType::VERTEX_ARRAY, m_VAO.GetID(), "Mesh_" + glName);
	}

	Mesh* Mesh::Create(const std::string& name)
	{
		Mesh* _mesh = new Mesh(name);

		AddToDatabase(name, _mesh);
		Message_Created(name, _mesh);

		return _mesh;
	}
	Mesh* Mesh::Create(const std::string& name, Model* _model)
	{
		Mesh* _mesh = new Mesh(name);

		AddToDatabase(name, _mesh);
		Message_Created(name, _mesh);

		_mesh->Set(_model);

		return _mesh;
	}

	Mesh::~Mesh()
	{
		
	}

	void Mesh::Set(Model* _model)
	{
		VXL_ASSERT(_model, "Model is nullptr");
		m_positions.set(_model->positions);
		m_uvs.set(_model->uvs);
		m_normals.set(_model->normals);
		m_tangents.set(_model->tangents);
		m_bitangents.set(_model->bitangents);
		//m_colors.set(_model->colors);
		m_indices.set(_model->indices);

		Bind();
	}

	void Mesh::GenerateNormals(
		Vector3* _vertices, uint32_t _vertCount,
		uint32_t* _indices, uint32_t _indexCount,
		bool smooth
	){
		if (_vertices == nullptr || _vertCount == 0)
		{
			VXL_ASSERT(false, "Vertices missing");
			return;
		}
		//
		std::vector<Vector3> Normals;
		Normals.resize(_vertCount);
		// Indexed Normals
		if (_indices)
		{
			VXL_ASSERT(_indexCount % 3 == 0 && _indexCount > 0, "Index needs to be a multiple of 3 and not empty");
			
			for (uint32_t i = 0; i < _indexCount; i+= 3)
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
				Vector3 N = Vector3::Cross((P2 - P1), (P3 - P1)).Normalize();
				Normals[_indices[i + 0]] += N;
				Normals[_indices[i + 1]] += N;
				Normals[_indices[i + 2]] += N;

			}
			if (smooth)
			{
				for (uint32_t i = 0; i < _indexCount; i ++)
				{
					std::vector<Vector3> neighbors;

					Vector3 P(
						_vertices[_indices[i]].x,
						_vertices[_indices[i]].y,
						_vertices[_indices[i]].z
					);

					for (uint32_t j = 0; j < _indexCount; j++)
					{
						if (i == j)
							continue;

						Vector3 Q(
							_vertices[_indices[j]].x,
							_vertices[_indices[j]].y,
							_vertices[_indices[j]].z
						);

						if (Vector3::Distance(P, Q) < 0.01f)
						{
							neighbors.push_back(Normals[_indices[j]]);
						}
					}

					// No smoothing occurs
					if (neighbors.size() == 1)
						continue;

					// Smooth vertices in similar space
					float neighborCount = (float)neighbors.size();

					Normals[_indices[i]] /= neighborCount;
					for (auto& n : neighbors)
						Normals[_indices[i]] += (n / neighborCount);

					if (isinf(Normals[_indices[i]].x))
						VXL_ASSERT(false, "Normal has invalid value");
				}
			}
		}
		// Position Normals
		else
		{
			VXL_ASSERT(_vertCount % 3 == 0, "Vertices is not a multiple of 3");

			for (uint32_t i = 0; i < _vertCount; i += 3)
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
				Vector3 N = Vector3::Cross((P2 - P1), (P3 - P1)).Normalize();
				Normals[i + 0] += N;
				Normals[i + 1] += N;
				Normals[i + 2] += N;
			}
			if (smooth)
			{
				std::vector<Vector3> WNormals;
				WNormals.reserve(_vertCount);
				WNormals.resize(_vertCount);

				for (uint32_t i = 0; i < _vertCount; i++)
				{
					WNormals[i] = Normals[i];

					std::vector<Vector3> neighbors;

					Vector3 P(
						_vertices[i].x,
						_vertices[i].y,
						_vertices[i].z
					);

					for (uint32_t j = 0; j < _vertCount; j++)
					{
						if (i == j)
							continue;

						Vector3 Q(
							_vertices[j].x,
							_vertices[j].y,
							_vertices[j].z
						);

						if (Vector3::Distance(P, Q) < 0.01f)
						{
							WNormals[i] += Normals[j];
							//neighbors.push_back(Normals[j]);
						}
					}
					//	
					//	// No smoothing occurs
					//	if (neighbors.size() == 1)
					//		continue;
					//	
					//	// Smooth vertices in similar space
					//	float neighborCount = (float)neighbors.size() + 1;
					//	
					//	//Normals[i] /= neighborCount;
					//	for (auto& n : neighbors)
					//		Normals[i] += n;// / neighborCount;
					//	
					//	Normals[i] /= neighborCount;
					//	
					//	//Normals[i] /= (neighborCount + 1);
					//	//Normals[i].NormalizeSelf();
					//	
					//	if (isinf(Normals[i].x))
					//	{
					//		float a = 10.0f;
					//	}
						//assert(false);
				}

				for (uint32_t i = 0; i < _vertCount; i++)
				{
					Normals[i] = WNormals[i];
				}
			}
		}
		// Normalize the normals
		for (uint32_t i = 0; i < _vertCount; i++)
		{
			Normals[i].NormalizeSelf();
		}


		//
		m_normals.set(Normals);
	}

	void Mesh::GenerateTangents(
		Vector3* _vertices, uint32_t _vertCount,
		Vector2* _uvs, uint32_t _UVCount,
		uint32_t* _indices, uint32_t _indexCount
	)
	{
		if (_vertices == nullptr || _vertCount == 0)
		{
			VXL_ASSERT(false, "Vertices are missing");
			return;
		}
		//
		std::vector<Vector3> Tangents;
		std::vector<Vector3> Bitangents;
		Tangents.resize(_vertCount);
		Bitangents.resize(_vertCount);
		// Indexed Normals
		if (_indices)
		{
			VXL_ASSERT(_indexCount % 3 == 0 && _indexCount > 0, "Indices are not multiple of 3 or are empty");

			for (uint32_t i = 0; i < _indexCount; i += 3)
			{
				// Positions
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
				
				// Uvs
				Vector2 UV1(
					_uvs[_indices[i + 0]].x,
					_uvs[_indices[i + 0]].y
				);
				Vector2 UV2(
					_uvs[_indices[i + 1]].x,
					_uvs[_indices[i + 1]].y
				);
				Vector2 UV3(
					_uvs[_indices[i + 2]].x,
					_uvs[_indices[i + 2]].y
				);

				// Edges of triangle
				Vector3 deltaPos1 = P2 - P1;
				Vector3 deltaPos2 = P3 - P1;
				// UV delta
				Vector2 deltaUV1 = UV2 - UV1;
				Vector2 deltaUV2 = UV3 - UV1;

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				Vector3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
				Vector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
				Tangents[_indices[i + 0]] += tangent;
				Tangents[_indices[i + 1]] += tangent;
				Tangents[_indices[i + 2]] += tangent;
				Bitangents[_indices[i + 0]] += bitangent;
				Bitangents[_indices[i + 1]] += bitangent;
				Bitangents[_indices[i + 2]] += bitangent;
			}
		}
		// Position Normals
		else
		{
			VXL_ASSERT(_vertCount % 3 == 0, "Vertices are not multiple of 3");

			for (uint32_t i = 0; i < _vertCount; i += 3)
			{
				// Positions
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

				// Uvs
				Vector2 UV1(
					_uvs[i + 0].x,
					_uvs[i + 0].y
				);
				Vector2 UV2(
					_uvs[i + 1].x,
					_uvs[i + 1].y
				);
				Vector2 UV3(
					_uvs[i + 2].x,
					_uvs[i + 2].y
				);

				// Edges of triangle
				Vector3 deltaPos1 = P2 - P1;
				Vector3 deltaPos2 = P3 - P1;
				// UV delta
				Vector2 deltaUV1 = UV2 - UV1;
				Vector2 deltaUV2 = UV3 - UV1;

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				Vector3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
				Vector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
				Tangents[i + 0] += tangent;
				Tangents[i + 1] += tangent;
				Tangents[i + 2] += tangent;
				Bitangents[i + 0] += bitangent;
				Bitangents[i + 1] += bitangent;
				Bitangents[i + 2] += bitangent;
			}
		}
		// Normalize the normals
		for (uint32_t i = 0; i < _vertCount; i++)
		{
			Tangents[i].NormalizeSelf();
			Bitangents[i].NormalizeSelf();
		}
		//
		m_tangents.set(Tangents);
		m_bitangents.set(Bitangents);
	}

	void Mesh::GenerateNormals(bool Smooth)
	{
		// Generate Missing Normals
		if (m_normals.Empty() && !m_positions.Empty())
		{
			GenerateNormals(
				m_positions.vertices.data(), (uint32_t)m_positions.vertices.size(),
				m_indices.indices.data(), (uint32_t)m_indices.indices.size(),
				Smooth
			);
		}
	}
	void Mesh::GenerateTangents()
	{
		// Generate Missing Tangents
		if ((m_tangents.Empty() || m_bitangents.Empty()) && !m_positions.Empty() && !m_uvs.Empty())
		{
			GenerateTangents(
				m_positions.vertices.data(), (uint32_t)m_positions.vertices.size(),
				m_uvs.vertices.data(), (uint32_t)m_uvs.vertices.size(),
				m_indices.indices.data(), (uint32_t)m_indices.indices.size()
			);
		}
	}

	void Mesh::Bind(DrawType type)
	{
		m_type = type;
		m_subtype = Graphics::GetDrawSubType(type);
		UpdateDrawInfo();

		// SIZE Assert Check //
#ifdef _DEBUG
		uint32_t indicesCount = m_indices.GetDrawCount();
		if (indicesCount)
		{
			if(m_subtype == DrawSubType::TRIANGLES)
			{
				VXL_ASSERT(indicesCount % 3 == 0, "Indices are not multiple of 3");
			}
			else if (m_subtype == DrawSubType::LINES)
			{
				VXL_ASSERT(indicesCount % 2 == 0, "Indices are not multiple of 2");
			}
		}
		else
		{
			uint32_t posCount = m_positions.GetDrawCount();
			if (m_subtype == DrawSubType::TRIANGLES)
			{
				VXL_ASSERT(posCount % 3 == 0, "Vertices are not multiple of 3");
			}
			else if (m_subtype == DrawSubType::LINES)
			{
				VXL_ASSERT(posCount % 2 == 0, "Vertices are not multiple of 2");
			}
		}
#endif

		/*	Bind Data	*/
		m_VAO.bind();

		m_positions.m_vbo.Bind();
		m_uvs.m_vbo.Bind();
		m_normals.m_vbo.Bind();
		m_tangents.m_vbo.Bind();
		m_bitangents.m_vbo.Bind();
		//m_colors.m_vbo.Bind();
		m_instances.m_vbo.Bind();
		m_indices.m_ebo.Bind();

		m_VAO.unbind();
		/*				*/	

		RecalculateMinMax();
	}

	void Mesh::RecalculateMinMax()
	{
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
		m_VAO.bind();
		(*this.*Draw_Function)();
	}
}