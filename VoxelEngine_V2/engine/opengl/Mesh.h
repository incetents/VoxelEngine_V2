// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"

#include "../utilities/logger.h"
#include "../utilities/Database.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

#include <Windows.h>
#include <vector>

namespace Vxl
{

	template<typename Type, UINT ValueCount, BufferType Buf>
	class MeshBuffer
	{
		friend class Mesh;
	private:
		VBO m_vbo;
	public:
		MeshBuffer()
		{
			m_vbo.AddStrideHint(Buf, ValueCount);
		}

		void set(Type* arr, GLuint count)
		{
			m_vbo.SetVertices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<Type> vec)
		{
			m_vbo.SetVertices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBuffer& operator=(std::vector<Type> vec)
		{
			set(vec);
			return *this;
		}

		inline UINT GetDrawCount(void) const
		{
			return m_vbo.GetDrawCount();
		}
		inline UINT GetSize(void) const
		{
			return m_vbo.GetSize();
		}
		inline bool Empty()
		{
			return m_vbo.GetSize() == 0;
		}
	};

	class MeshBufferVertices
	{
		friend class Mesh;
	private:
		VBO m_vbo;
		std::vector<Vector3> vertices;
	public:
		MeshBufferVertices()
		{
			m_vbo.AddStrideHint(Vxl::BufferType::VERTEX, 3);
		}

		void set(Vector3* arr, GLuint count)
		{
			vertices.clear();
			vertices = std::vector<Vector3>(arr, arr + count);
			m_vbo.SetVertices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<Vector3> vec)
		{
			vertices = vec;
			m_vbo.SetVertices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBufferVertices& operator=(std::vector<Vector3> vec)
		{
			set(vec);
			return *this;
		}

		inline UINT GetDrawCount(void) const
		{
			return m_vbo.GetDrawCount();
		}
		inline UINT GetSize(void) const
		{
			return m_vbo.GetSize();
		}
		inline bool Empty()
		{
			return m_vbo.GetSize() == 0;
		}
	};

	class MeshBufferIndices
	{
		friend class Mesh;
	private:
		EBO m_ebo;
		std::vector<GLuint> indices;
	public:
		void set(GLuint* arr, GLuint count)
		{
			m_ebo.SetIndices(arr, count, BufferBind_Mode::STATIC);
			indices.clear();
			indices = std::vector<GLuint>(arr, arr + count);
		}
		void set(std::vector<GLuint> vec)
		{
			m_ebo.SetIndices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
			indices.clear();
			indices = vec;
		}
		MeshBufferIndices& operator=(std::vector<GLuint> vec)
		{
			set(vec);
			return *this;
		}

		inline UINT GetDrawCount(void) const
		{
			return m_ebo.GetDrawCount();
		}
		inline UINT GetSize(void) const
		{
			return m_ebo.GetSize();
		}
		inline bool Empty()
		{
			return m_ebo.GetSize() == 0;
		}
	};

	class MeshBufferInstancing
	{
		friend class Mesh;
	private:
		VBO m_vbo;
	public:
		MeshBufferInstancing()
		{
			m_vbo.AddStrideHint(BufferType::InstancingStart, 0);
		}

		void set(Matrix4x4* arr, GLuint count)
		{
			m_vbo.SetVertices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<Matrix4x4> vec)
		{
			m_vbo.SetVertices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBufferInstancing& operator=(std::vector<Matrix4x4> vec)
		{
			set(vec);
			return *this;
		}

		inline UINT GetDrawCount(void) const
		{
			return m_vbo.GetDrawCount();
		}
		inline UINT GetSize(void) const
		{
			return m_vbo.GetSize();
		}
		inline bool Empty()
		{
			return m_vbo.GetSize() == 0;
		}
	};

	class Mesh
	{
	private:
		GLuint  m_VAO;

		Draw_Type m_type = Draw_Type::TRIANGLES;
		Draw_Mode m_mode = Draw_Mode::ARRAY;
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

		static Database<Mesh> m_database;
	public:
		// Stores Mesh in Database optionally
		static Mesh* Create(const std::string& name);
		// Get Mesh from database
		static Mesh* Get(const std::string& name)
		{
			return m_database.Get(name);
		}

		Mesh();
		virtual ~Mesh();

		MeshBufferVertices m_positions;
		MeshBuffer<Vector2, 2, BufferType::UV> m_uvs;
		MeshBuffer<Vector3, 3, BufferType::NORMAL> m_normals;
		MeshBuffer<Vector3, 3, BufferType::TANGENT> m_tangents;
		MeshBuffer<Vector3, 3, BufferType::BITANGENT> m_bitangents;
		MeshBuffer<Vector3, 4, BufferType::COLOR> m_colors;
		MeshBufferInstancing m_instances;
		MeshBufferIndices m_indices;

		// Fills m_normals Based on existing positions and/or indices
		void GenerateNormals(
			Vector3* _vertices, GLuint _vertCount,
			GLuint* _indices = nullptr, GLuint _indexCount = 0
		);

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

		void Bind(Draw_Type type = Draw_Type::TRIANGLES);
		void RecalculateMinMax();

		void Draw();
	};
}

