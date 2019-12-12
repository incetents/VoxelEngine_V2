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

	//
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
		Vector3		m_center; // (max + min) / 2
		Vector3		m_scale;  // (max - min)

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

		// Protected, created through assets
		Mesh() {}
	public:

		virtual ~Mesh() {}

		MeshBuffer<Vector3> m_positions =	MeshBuffer<Vector3>(BufferLayout({ {AttributeLocation::LOC0, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		MeshBuffer<Vector2> m_uvs =		MeshBuffer<Vector2>(BufferLayout({ {AttributeLocation::LOC1, AttributeType::VEC2} }), BufferUsage::STATIC_DRAW);
		MeshBuffer<Vector3> m_normals =	MeshBuffer<Vector3>(BufferLayout({ {AttributeLocation::LOC2, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		MeshBuffer<Vector3> m_tangents =	MeshBuffer<Vector3>(BufferLayout({ {AttributeLocation::LOC6, AttributeType::VEC3} }), BufferUsage::STATIC_DRAW);
		MeshBuffer<Matrix4x4> m_instances = MeshBuffer<Matrix4x4>(BufferLayout(
			{
				{AttributeLocation::LOC8, AttributeType::VEC4, false, 1},
				{AttributeLocation::LOC9, AttributeType::VEC4, false, 1},
				{AttributeLocation::LOC10, AttributeType::VEC4, false, 1},
				{AttributeLocation::LOC11, AttributeType::VEC4, false, 1}
			}), BufferUsage::STATIC_DRAW);

		MeshBufferIndices m_indices = MeshBufferIndices(BufferUsage::STATIC_DRAW);

		// Update all data from model
		void set(const Model& _model);
		void setGLName(const std::string& name);

		inline uint32_t		getDrawCount(void)	 const
		{
			return m_drawCount;
		}
		inline DrawType		getDrawType(void)	 const
		{
			return m_type;
		}
		inline DrawSubType	getDrawSubType(void) const
		{
			return m_subtype;
		}
		inline DrawMode		getDrawMode(void)	 const
		{
			return m_mode;
		}
		inline Vector3		getVertexMin(void)	 const
		{
			return m_min;
		}
		inline Vector3		getVertexMax(void)	 const
		{
			return m_max;
		}
		inline Vector3		getCenter(void)const
		{
			return m_center;
		}
		inline Vector3		getScale(void)const
		{
			return m_scale;
		}
		inline VAOID		getVAOID(void) const
		{
			return m_VAO.getID();
		}

		void generateNormals(bool Smooth);
		void generateTangents();
		void recalculateMinMax();

		void bind(DrawType type = DrawType::TRIANGLES);
		void draw();
	};

	//
	class LineMesh3D
	{
	private:
		VAO m_VAO;

		DrawType	m_type;
		DrawSubType m_subtype; // points, lines, or triangles
		uint32_t	m_drawCount = 0;	// Vertices Drawn

		const static uint32_t m_indexIncrement; // Floats per line set

	public:
		MeshBuffer<float> m_points = MeshBuffer<float>(BufferLayout(
			{
				{AttributeLocation::LOC0, AttributeType::VEC3 }, // Position
				{AttributeLocation::LOC3, AttributeType::VEC3 }, // Color
				{AttributeLocation::LOC1, AttributeType::FLOAT}  // Width
			}),
			BufferUsage::DYNAMIC_DRAW
			);
		
		// Used to reference where in points the vertices is being used
		uint32_t m_index = 0;

		void addLine(const Vector3& P1, const Vector3& P2, float Width, const Color3F& C1, const Color3F& C2);

		void clear();
		void bind(DrawType type = DrawType::LINES);
		void draw();
	};
	const uint32_t LineMesh3D::m_indexIncrement = 14;

	//
	class LineMesh2D
	{
	private:
		VAO m_VAO;

		DrawType	m_type;
		DrawSubType m_subtype; // points, lines, or triangles
		uint32_t	m_drawCount = 0;	// Vertices Drawn

		const static uint32_t m_indexIncrement; // Floats per line set

	public:
		MeshBuffer<float> m_points = MeshBuffer<float>(BufferLayout(
			{
				{AttributeLocation::LOC0, AttributeType::VEC2 }, // Position
				{AttributeLocation::LOC3, AttributeType::VEC3 }, // Color
				{AttributeLocation::LOC1, AttributeType::FLOAT}  // Width
			}),
			BufferUsage::DYNAMIC_DRAW
			);

		// Used to reference where in points the vertices is being used
		uint32_t m_index = 0;

		void addLine(const Vector2& P1, const Vector2& P2, float Width, const Color3F& C1, const Color3F& C2);

		void clear();
		void bind(DrawType type = DrawType::LINES);
		void draw();
	};
	const uint32_t LineMesh2D::m_indexIncrement = 12;
}

