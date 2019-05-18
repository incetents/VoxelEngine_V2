// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"

#include "../utilities/logger.h"
#include "../utilities/Asset.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"
#include "../math/Model.h"

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
			m_vbo.SetVertices(vec.data(), (GLuint)vec.size(), BufferBind_Mode::STATIC);
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

	// MeshBuffer except it will remember the array that is passed through
	template<typename Type, UINT ValueCount, BufferType Buf>
	class MeshBufferMem
	{
		friend class Mesh;
	private:
		VBO m_vbo;
		bool alloc = false;
		std::vector<Type>* vertices = nullptr;
	public:
		MeshBufferMem()
		{
			vertices = new std::vector<Type>();
			m_vbo.AddStrideHint(Buf, ValueCount);
		}

		void setAlloc(std::vector<Type>* arr)
		{
			if (alloc)
				delete vertices;
			alloc = false;
			vertices = arr;
		}
		void set(Type* arr, GLuint count)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Type>(arr, arr + count);

			m_vbo.SetVertices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<Type> vec)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Type>(vec);

			m_vbo.SetVertices(vec.data(), (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBufferMem& operator=(std::vector<Type> vec)
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
		bool alloc = false;
		std::vector<Matrix4x4>* vertices = nullptr;
	public:
		MeshBufferInstancing()
		{
			vertices = new std::vector<Matrix4x4>();
			m_vbo.AddStrideHint(BufferType::InstancingStart, 0);
		}

		void setAlloc(std::vector<Matrix4x4>* arr)
		{
			if (alloc)
				delete vertices;
			alloc = false;
			vertices = arr;
		}
		void set(Matrix4x4* arr, GLuint count)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Matrix4x4>(arr, arr + count);

			m_vbo.SetVertices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<Matrix4x4> vec)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Matrix4x4>(vec);

			m_vbo.SetVertices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBufferInstancing& operator=(std::vector<Matrix4x4> vec)
		{
			set(vec);
			return *this;
		}

		inline std::vector<Matrix4x4>* GetVertices(void) const
		{
			return vertices;
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

	class Mesh : public Asset<Mesh>
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

		MeshBufferMem<Vector3, 3, BufferType::POSITION> m_positions;
		MeshBufferMem<Vector2, 2, BufferType::UV> m_uvs;
		MeshBuffer<Vector3, 3, BufferType::NORMAL> m_normals;
		MeshBuffer<Vector3, 3, BufferType::TANGENT> m_tangents;
		MeshBuffer<Vector3, 3, BufferType::BITANGENT> m_bitangents;
		MeshBuffer<Vector4, 4, BufferType::COLOR> m_colors;
		MeshBufferInstancing m_instances;
		MeshBufferIndices m_indices;

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
		void Bind(Draw_Type type = Draw_Type::TRIANGLES);
		void RecalculateMinMax();

		void Draw();
	};
}

