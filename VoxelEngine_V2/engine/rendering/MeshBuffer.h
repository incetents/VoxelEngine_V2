// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

namespace Vxl
{
	template<typename Type, BufferType Buf, AttributeType Atr>
	class MeshBuffer
	{
		friend class Mesh;
		friend class LineMesh;
	protected:
		VBO m_vbo;
	public:
		MeshBuffer()
		{
			if (Buf == BufferType::NONE || Atr == AttributeType::NONE)
				VXL_ASSERT(false, "MeshBuffer Layout cannot be None");

			BufferLayout layout =
			{
				{Buf, Atr, false}
			};
			m_vbo.SetLayout(layout);
		}


		virtual void set(Type* arr, uint32_t size, BufferUsage mode = BufferUsage::STATIC_DRAW)
		{
			m_vbo.SetVertices(arr, size, mode);
		}
		virtual void set(const std::vector<Type>& vec, BufferUsage mode = BufferUsage::STATIC_DRAW)
		{
			m_vbo.SetVertices(vec.data(), (uint32_t)vec.size(), mode);
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

	template<typename Type, BufferType Buf, AttributeType Atr>
	class MeshBufferMem : public MeshBuffer<Type, Buf, Atr>
	{
		friend class Mesh;
	private:
		std::vector<Type> vertices;
	public:
		MeshBufferMem(){}
		 
		void set(Type* arr, uint32_t size, BufferUsage mode = BufferUsage::STATIC_DRAW) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, Atr>::m_vbo.SetVertices(arr, size, mode);
		}
		void set(const std::vector<Type>& vec, BufferUsage mode = BufferUsage::STATIC_DRAW) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, Atr>::m_vbo.SetVertices(vec.data(), (uint32_t)vec.size(), mode);
		}

		void update(Type* arr, int offset, int size) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, Atr>::m_vbo.UpdateVertices(arr, offset, size);
		}
		void update(const std::vector<Type>& vec, int offset, int size) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type, Buf, Atr>::m_vbo.UpdateVertices(&vec[0], offset, size);
		}

		void update(Type* arr) override
		{
			vertices = std::vector<Type>(arr, arr + vertices.size());
			vertices.shrink_to_fit();
		
			MeshBuffer<Type, Buf, Atr>::m_vbo.UpdateVertices(arr, 0);
		}
		void update(const std::vector<Type>& vec) override
		{
			vertices = vec;
			vertices.shrink_to_fit();
		
			MeshBuffer<Type, Buf, Atr>::m_vbo.UpdateVertices(&vec[0], 0);
		}

	};

	class MeshBufferIndices
	{
		friend class Mesh;
	private:
		EBO m_ebo;
		std::vector<uint32_t> indices;
	public:
		void set(uint32_t* arr, uint32_t size)
		{
			indices = std::vector<uint32_t>(arr, arr + size);
			indices.shrink_to_fit();

			m_ebo.SetIndices(arr, size, BufferUsage::STATIC_DRAW);
		}
		void set(std::vector<uint32_t> vec)
		{
			indices = vec;
			indices.shrink_to_fit();

			m_ebo.SetIndices(&vec[0], (uint32_t)vec.size(), BufferUsage::STATIC_DRAW);
		}
		MeshBufferIndices& operator=(std::vector<uint32_t> vec)
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
				{BufferType::INSTANCING_1, AttributeType::VEC4, false, 1},
				{BufferType::INSTANCING_2, AttributeType::VEC4, false, 1},
				{BufferType::INSTANCING_3, AttributeType::VEC4, false, 1},
				{BufferType::INSTANCING_4, AttributeType::VEC4, false, 1}
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
		void set(Matrix4x4* arr, uint32_t count)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Matrix4x4>(arr, arr + count);

			m_vbo.SetVertices(arr, count, BufferUsage::STATIC_DRAW);
		}
		void set(std::vector<Matrix4x4> vec)
		{
			if (alloc)
				delete vertices;
			alloc = true;
			vertices = new std::vector<Matrix4x4>(vec);

			m_vbo.SetVertices(&vec[0], (uint32_t)vec.size(), BufferUsage::STATIC_DRAW);
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

