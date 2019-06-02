// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
#include "glUtil.h"

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

		Draw_Type m_type; // Triangles = Default
		Draw_Mode m_mode; // Array = Default
		GLuint	  m_drawCount = 0;	// Vertices Drawn
		GLuint	  m_faces = 0;		// Triangles Drawn
		GLuint	  m_lines = 0;		// Lines Drawn

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
			Vector3* _vertices, GLuint _vertCount,
			GLuint* _indices = nullptr, GLuint _indexCount = 0,
			bool smooth = false
		);
		// Fills m_tangents and m_bitangents based on existing positions/uvs/indices
		void GenerateTangents(
			Vector3* _vertices, GLuint _vertCount,
			Vector2* _uvs, GLuint _UVCount,
			GLuint* _indices = nullptr, GLuint _indexCount = 0
		);

		Mesh(const std::string& glName = "");
	public:
		// Stores Mesh in Database optionally
		static Mesh* Create(const std::string& name);
		static Mesh* Create(const std::string& name, Model* model);
		virtual ~Mesh();

		MeshBufferMem<Vector3, BufferType::POSITION, 3> m_positions;
		MeshBufferMem<Vector2, BufferType::UV, 2>		m_uvs;
		MeshBuffer<Vector3, BufferType::NORMAL, 3>		m_normals;
		MeshBuffer<Vector3, BufferType::TANGENT, 3>		m_tangents;
		MeshBuffer<Vector3, BufferType::BITANGENT, 3>	m_bitangents;
		MeshBuffer<Vector4, BufferType::COLOR, 4>		m_colors;
		MeshBufferInstancing							m_instances;
		MeshBufferIndices								m_indices;

		// Update all data from model
		void Set(Model* _model);

		inline GLuint		GetDrawCount(void) const
		{
			return m_drawCount;
		}
		inline Draw_Type	GetDrawType(void) const
		{
			return m_type;
		}
		inline Draw_Mode	GetDrawMode(void) const
		{
			return m_mode;
		}
		inline Vector3		GetVertexMin(void) const
		{
			return m_min;
		}
		inline Vector3		GetVertexMax(void) const
		{
			return m_max;
		}

		void GenerateNormals(bool Smooth);
		void GenerateTangents();
		void RecalculateMinMax();

		void Bind(Draw_Type type = Draw_Type::TRIANGLES);
		void Draw();
	};
}

