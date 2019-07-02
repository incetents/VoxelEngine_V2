// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

namespace Vxl
{
	template<typename Type, BufferType Buf, ShaderDataType SDT>
	class MeshBuffer
	{
		friend class Mesh;
		friend class LineMesh;
	protected:
		VBO m_vbo;
	public:
		MeshBuffer()
		{
			if (Buf == BufferType::NONE || SDT == ShaderDataType::NONE)
				VXL_ASSERT(false, "MeshBuffer Layout cannot be None");

			BufferLayout layout =
			{
				{Buf, SDT, false}
			};
			m_vbo.SetLayout(layout);
		}


		virtual void set(Type* arr, GLuint size, BufferBind_Mode mode = BufferBind_Mode::STATIC)
		{
			m_vbo.SetVertices(arr, size, mode);
		}
		virtual void set(const std::vector<Type>& vec, BufferBind_Mode mode = BufferBind_Mode::STATIC)
		{
			m_vbo.SetVertices(vec.data(), (GLuint)vec.size(), mode);
		}

		virtual void update(Type* arr, int offset, int size)
		{
			m_vbo.UpdateVertices(arr, offset, size);
		}
		virtual void update(const std::vector<Type>& vec, int offset, int size)
		{
			m_vbo.UpdateVertices(&vec[0], offset, size);
		}

		virtual void update(Type* arr)
		{
			m_vbo.UpdateVertices(arr, 0);
		}
		virtual void update(const std::vector<Type>& vec)
		{
			m_vbo.UpdateVertices(&vec[0], 0);
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
			return m_vbo.IsEmpty();
		}
	};

	template<typename Type, BufferType Buf, ShaderDataType SDT>
	class MeshBufferMem : public MeshBuffer<Type, Buf, SDT>
	{
		friend class Mesh;
	private:
		std::vector<Type> vertices;
	public:
		MeshBufferMem(){}
		 
		void set(Type* arr, GLuint size, BufferBind_Mode mode = BufferBind_Mode::STATIC) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, SDT>::m_vbo.SetVertices(arr, size, mode);
		}
		void set(const std::vector<Type>& vec, BufferBind_Mode mode = BufferBind_Mode::STATIC) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, SDT>::m_vbo.SetVertices(vec.data(), (GLuint)vec.size(), mode);
		}

		void update(Type* arr, int offset, int size) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, SDT>::m_vbo.UpdateVertices(arr, offset, size);
		}
		void update(const std::vector<Type>& vec, int offset, int size) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, SDT>::m_vbo.UpdateVertices(&vec[0], offset, size);
		}

		void update(Type* arr) override
		{
			vertices = std::vector<Type>(arr, arr + vertices.size());
			vertices.shrink_to_fit();
		
			MeshBuffer<Type, Buf, SDT>::m_vbo.UpdateVertices(arr, 0);
		}
		void update(const std::vector<Type>& vec) override
		{
			vertices = vec;
			vertices.shrink_to_fit();
		
			MeshBuffer<Type, Buf, SDT>::m_vbo.UpdateVertices(&vec[0], 0);
		}

	};

	class MeshBufferIndices
	{
		friend class Mesh;
	private:
		EBO m_ebo;
		std::vector<GLuint> indices;
	public:
		void set(GLuint* arr, GLuint size)
		{
			indices = std::vector<GLuint>(arr, arr + size);
			indices.shrink_to_fit();

			m_ebo.SetIndices(arr, size, BufferBind_Mode::STATIC);
		}
		void set(std::vector<GLuint> vec)
		{
			indices = vec;
			indices.shrink_to_fit();

			m_ebo.SetIndices(&vec[0], (GLuint)vec.size(), BufferBind_Mode::STATIC);
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
			return m_ebo.IsEmpty();
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

			// Instancing must be seperated into 4 vec4s
			BufferLayout layout =
			{
				{BufferType::INSTANCING_1, ShaderDataType::VEC4, false, 1},
				{BufferType::INSTANCING_2, ShaderDataType::VEC4, false, 1},
				{BufferType::INSTANCING_3, ShaderDataType::VEC4, false, 1},
				{BufferType::INSTANCING_4, ShaderDataType::VEC4, false, 1}
			};
			m_vbo.SetLayout(layout);
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

}

