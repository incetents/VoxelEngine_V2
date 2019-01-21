// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"

#include "../utilities/logger.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

#include <Windows.h>
#include <vector>

namespace Vxl
{
	/*
	class MeshBufferBase
	{
		friend class Mesh;
	protected:
		bool	m_isInit	= false;
		GLvoid*	m_array		= nullptr;
		GLuint	m_length	= 0;
		GLuint* m_VBO_ref	= nullptr;
		GLuint  m_VBO_index = -1;

		BufferBind_Mode  m_bindMode = BufferBind_Mode::STATIC;

		virtual void bind() = 0;

		template<typename Type>
		void set(Type* arr, GLuint length)
		{
			clear();
			m_array = new Type[length];
			m_length = length;
			memcpy(m_array, arr, length * sizeof(Type));
		}

	public:
		inline GLuint getLength() const
		{
			return m_length;
		}
		inline GLuint getVBOIndex() const
		{
			return m_VBO_index;
		}
		
		void clear()
		{
			if(m_array)
				delete[] m_array;

			m_array = nullptr;
			m_length = 0;
		}
	};


	template<typename Type>
	class MeshBuffer : public MeshBufferBase
	{
		friend class Mesh;
	private:
		void bind() override
		{
			// Nothing to bind if it has no data
			if (m_length == 0)
				return;

			if (!m_isInit)
			{
				Logger.error("Cannot Bind Mesh Buffer because it is not initialiased");
				return;
			}

			// Send Array Of Data
			glUtil::bindArray(*m_VBO_ref, m_length * sizeof(Type), &((Type*)m_array)[0], (GLenum)m_bindMode);

			// Vertex Attribute Number
			glUtil::setVertexAttrib((GLuint)m_VBO_index, sizeof(Type) / 4, DataType::FLOAT);

		}

	public:

		void setup(GLuint& vbo, GLuint vbo_index)
		{
			m_VBO_ref = &vbo;
			m_VBO_index = vbo_index;
			m_isInit = true;
		}

		void set(Type* arr, GLuint length)
		{
			MeshBufferBase::set<Type>(arr, length);
		}
		void set(std::vector<Type>& vector)
		{
			MeshBufferBase::set<Type>(&vector[0], (GLuint)vector.size());
		}
		MeshBuffer& operator=(std::vector<Type>& vector)
		{
			MeshBuffer::set(vector);
			return *this;
		}

		
	};

	class MeshBufferInstancing : public MeshBufferBase
	{
		friend class Mesh;
	private:
		void bind() override
		{
			// Nothing to bind if it has no data
			if (m_length == 0)
				return;

			if (!m_isInit)
			{
				Logger.error("Cannot Bind Mesh Buffer because it is not initialiased");
				return;
			}

			// Send Array Of Data
			glUtil::bindArray(*m_VBO_ref, m_length * sizeof(Matrix4x4), &((Matrix4x4*)m_array)[0], (GLenum)m_bindMode);

			// Vertex Attribute Number
			glUtil::setVertexAttribInstancing((GLuint)m_VBO_index);

		}

	public:

		void setup(GLuint& vbo, GLuint vbo_index)
		{
			m_VBO_ref = &vbo;
			m_VBO_index = vbo_index;
			m_isInit = true;
		}

		void set(Matrix4x4* arr, GLuint length)
		{
			MeshBufferBase::set<Matrix4x4>(arr, length);
		}
		void set(std::vector<Matrix4x4>& vector)
		{
			MeshBufferBase::set<Matrix4x4>(&vector[0], (GLuint)vector.size());
		}
		MeshBufferInstancing& operator=(std::vector<Matrix4x4>& vector)
		{
			MeshBufferInstancing::set(vector);
			return *this;
		}

	};

	class MeshBufferIndices : public MeshBufferBase
	{
		friend class Mesh;
	private:
		void bind() override
		{
			if (!m_isInit)
			{
				Logger.error("Cannot Bind Mesh Buffer Indices because it is not initialiased");
				return;
			}

			// Send Array of Indices
			glUtil::bindIndices(*m_VBO_ref, m_length * sizeof(GLuint), &((GLuint*)m_array)[0], (GLenum)m_bindMode);
		}
	public:

		void setup(GLuint& vbo)
		{
			m_VBO_ref = &vbo;
			m_isInit = true;
		}

		void set(GLuint* arr, GLuint length)
		{
			MeshBufferBase::set<GLuint>(arr, length);
		}
		void set(std::vector<GLuint>& vector)
		{
			MeshBufferBase::set<GLuint>(&vector[0], (GLuint)vector.size());
		}
		MeshBufferIndices& operator=(std::vector<GLuint>& vector)
		{
			MeshBufferIndices::set(vector);
			return *this;
		}

	};
	*/

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
	};

	class MeshBufferIndices
	{
		friend class Mesh;
	private:
		VBOI m_vboi;
	public:
		void set(GLuint* arr, GLuint count)
		{
			m_vboi.SetIndices(arr, count, BufferBind_Mode::STATIC);
		}
		void set(std::vector<GLuint> vec)
		{
			m_vboi.SetIndices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
		}
		MeshBufferIndices& operator=(std::vector<GLuint> vec)
		{
			set(vec);
			return *this;
		}

		inline UINT GetDrawCount(void) const
		{
			return m_vboi.GetDrawCount();
		}
		inline UINT GetSize(void) const
		{
			return m_vboi.GetSize();
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
	};

	class Mesh
	{
	protected:
		GLuint  m_VAO;

		Draw_Type m_type = Draw_Type::TRIANGLES;
		Draw_Mode m_mode = Draw_Mode::ARRAY;
		GLuint	  m_drawCount = 0;	// Vertices Drawn
		GLuint	  m_faces = 0;		// Triangles Drawn
		GLuint	  m_lines = 0;		// Lines Drawn

		void (Mesh::*Draw_Function)(void) = &Mesh::DrawArray;

		void DrawArray();
		void DrawIndexed();
		void DrawArrayInstances();
		void DrawIndexedInstances();

		void UpdateDrawInfo();

	public:
		Mesh();
		virtual ~Mesh();

		MeshBuffer<Vector3, 3, BufferType::VERTEX> m_positions;
		MeshBuffer<Vector2, 2, BufferType::UV> m_uvs;
		MeshBuffer<Vector3, 3, BufferType::NORMAL> m_normals;
		MeshBufferInstancing m_instances;
		MeshBufferIndices m_indices;

		// Fills m_normals Based on existing positions and/or indices
		void GenerateNormals(
			Vector3* _vertices, GLuint _vertCount,
			GLuint* _indices = nullptr, GLuint _indexCount = 0
		);

		inline GLuint		GetDrawCount() const
		{
			return m_drawCount;
		}
		inline Draw_Type	GetDrawType() const
		{
			return m_type;
		}
		inline Draw_Mode	GetDrawMode() const
		{
			return m_mode;
		}

		void Bind(Draw_Type type = Draw_Type::TRIANGLES);

		void Draw();
	};
}

