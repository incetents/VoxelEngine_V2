// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
#include "Graphics.h"

#include "../utilities/Types.h"
#include "../utilities/Macros.h"

#include <Windows.h>
#include <vector>

namespace Vxl
{
	class Model;

	class Mesh
	{
		DISALLOW_COPY_AND_ASSIGN(Mesh);
		friend class _Assets;
	private:
		VAO m_VAO;

		DrawType	m_type; // Triangles = Default
		DrawSubType m_subtype; // points, lines, or triangles
		DrawMode	m_mode; // Array = Default
		uint32_t	m_drawCount = 0;	// Vertices Drawn
		uint32_t	m_faces = 0;		// Triangles Drawn
		uint32_t	m_lines = 0;		// Lines Drawn
		Vector3		m_min; // smallest vertices of mesh
		Vector3		m_max; // largest vertices of mesh

		void UpdateDrawInfo();

		// Fills m_normals Based on existing positions and/or indices
		void GenerateNormals(
			const Vector3* _vertices, uint32_t _vertCount,
			const uint32_t* _indices = nullptr, uint32_t _indexCount = 0,
			bool smooth = false
		);
		// Fills m_tangents and m_bitangents based on existing positions/uvs/indices
		void GenerateTangents(
			const Vector3* _vertices, uint32_t _vertCount,
			const Vector2* _uvs, uint32_t _UVCount,
			const uint32_t* _indices = nullptr, uint32_t _indexCount = 0
		);

		Mesh();
	public:

		virtual ~Mesh() {}

		//	_MeshBuffer<Vector3> positions = _MeshBuffer<Vector3>(BufferLayout({ {BufferType::POSITION, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		//	_MeshBuffer<Vector2> uvs = _MeshBuffer<Vector2>(BufferLayout({ {BufferType::UV, AttributeType::VEC2} }), BufferUsage::STATIC_DRAW);
		//	_MeshBuffer<Vector3> normals = _MeshBuffer<Vector3>(BufferLayout({ {BufferType::NORMAL, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		//	_MeshBuffer<Vector3> tangents = _MeshBuffer<Vector3>(BufferLayout({ {BufferType::TANGENT, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		//	
		//	_MeshBufferIndices indices = _MeshBufferIndices(BufferUsage::STATIC_DRAW);

		MeshBufferMem<Vector3>	m_positions;
		MeshBufferMem<Vector2>	m_uvs;
		MeshBuffer<Vector3>		m_normals;
		MeshBuffer<Vector3>		m_tangents;
		MeshBufferInstancing	m_instances;
		MeshBufferIndices		m_indices;

		// Update all data from model
		void Set(const Model& _model);
		void setGLName(const std::string& name);

		inline uint32_t		GetDrawCount(void)	 const
		{
			return m_drawCount;
		}
		inline DrawType		GetDrawType(void)	 const
		{
			return m_type;
		}
		inline DrawSubType	GetDrawSubType(void) const
		{
			return m_subtype;
		}
		inline DrawMode		GetDrawMode(void)	 const
		{
			return m_mode;
		}
		inline Vector3		GetVertexMin(void)	 const
		{
			return m_min;
		}
		inline Vector3		GetVertexMax(void)	 const
		{
			return m_max;
		}
		inline VAOID		GetVAOID(void) const
		{
			return m_VAO.getID();
		}

		void GenerateNormals(bool Smooth);
		void GenerateTangents();
		void RecalculateMinMax();

		void bind(DrawType type = DrawType::TRIANGLES);
		void Draw();
	};
}

