// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
#include "Graphics.h"

#include "../utilities/Asset.h"

#include <Windows.h>
#include <vector>

namespace Vxl
{
	class Model;

	class Mesh : public Asset<Mesh>
	{
	private:
		VAO m_VAO;

		DrawType m_type; // Triangles = Default
		DrawSubType m_subtype; //
		DrawMode m_mode; // Array = Default
		uint32_t m_drawCount = 0;	// Vertices Drawn
		uint32_t m_faces = 0;		// Triangles Drawn
		uint32_t m_lines = 0;		// Lines Drawn

		Vector3 m_min; // smallest vertices of mesh
		Vector3 m_max; // largest vertices of mesh
		
		void (Mesh::*Draw_Function)(void) = &Mesh::DrawArray;

		void DrawArray();
		void DrawIndexed();
		void DrawArrayInstances();
		void DrawIndexedInstances();

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

		Mesh(const std::string& glName = "");
	public:
		// Stores Mesh in Database optionally
		static Mesh* Create();
		static Mesh* Create(const std::string& name);
		static Mesh* Create(const std::string& name, Model* model);
		virtual ~Mesh();

		MeshBufferMem<Vector3>	m_positions;
		MeshBufferMem<Vector2>	m_uvs;
		MeshBuffer<Vector3>		m_normals;
		MeshBuffer<Vector3>		m_tangents;
		MeshBuffer<Vector3>		m_bitangents;
		MeshBufferInstancing	m_instances;
		MeshBufferIndices		m_indices;

		// Update all data from model
		void Set(Model* _model);

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

		void GenerateNormals(bool Smooth);
		void GenerateTangents();
		void RecalculateMinMax();

		void Bind(DrawType type = DrawType::TRIANGLES);
		void Draw();
	};
}

