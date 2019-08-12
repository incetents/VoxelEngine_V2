// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

namespace Vxl
{
	// Contains VBO [doesn't remember any vertices sent]
	template<typename Type>
	class MeshBuffer
	{
	protected:
		VBO m_vbo;
		BufferUsage m_bindMode = BufferUsage::STATIC_DRAW;
		bool m_hasLayout = false;
	public:

		// Only applies changes on calling the "Set" function
		void setBindMode(BufferUsage usage)
		{
			m_bindMode = usage;
		}

		virtual void setLayout(const BufferLayout& layout)
		{
			m_hasLayout = true;
			m_vbo.SetLayout(layout);
		}

		virtual void bind(void) const
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before binding");
			m_vbo.Bind();
		}

		virtual void set(Type* arr, uint32_t size)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.SetVertices(arr, size, m_bindMode);
		}
		virtual void set(const std::vector<Type>& vec)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.SetVertices(vec.data(), (uint32_t)vec.size(), m_bindMode);
		}

		virtual void update(Type* arr)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.UpdateVertices(arr, 0);
		}
		virtual void update(Type* arr, int offset, int size)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.UpdateVertices(arr, offset, size);
		}

		virtual void update(const std::vector<Type>& vec)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.UpdateVertices(vec.data(), 0);
		}
		virtual void update(const std::vector<Type>& vec, int offset, int size)
		{
			VXL_ASSERT(m_hasLayout, "VBO Requires Layout before modifying");

			m_vbo.UpdateVertices(vec.data(), offset, size);
		}

		MeshBuffer<Type>& operator=(std::vector<uint32_t> vec)
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
		inline bool IsEmpty() const
		{
			return m_vbo.IsEmpty();
		}
	};

	// Contains VBO + vector for vertices
	template<typename Type>
	class MeshBufferMem
	{
	public:
		VBO m_vbo;
		BufferUsage m_bindMode = BufferUsage::STATIC_DRAW;
		bool m_hasLayout = false;

		std::vector<Type> vertices;
		bool dirtyVertices = false;
		bool resizedVertices = false;
	public:
		 
		// Only applies changes on calling the "Set" function
		void setBindMode(BufferUsage usage)
		{
			m_bindMode = usage;
		}

		virtual void setLayout(const BufferLayout& layout)
		{
			m_hasLayout = true;
			m_vbo.SetLayout(layout);
		}

		std::vector<Type>& getVertices(void)
		{
			return vertices;
		}
		uint32_t getVertexCount(void) const
		{
			return (uint32_t)vertices.size();
		}

		void setVerticesDirty(bool sizeHasChanged)
		{
			dirtyVertices = true;
			resizedVertices = sizeHasChanged;
		}

		void bind(void)
		{
			VXL_ASSERT(m_hasLayout, "MeshBufferMem missing Layout");

			if (dirtyVertices)
			{
				if(resizedVertices || m_vbo.IsEmpty())
					m_vbo.SetVertices(vertices, m_bindMode);
				else
					m_vbo.UpdateVertices(vertices.data(), 0);
			}

			m_vbo.Bind();

			dirtyVertices = false;
			resizedVertices = false;
		}

		// Set all vertices to zeroes
		void setZeroes()
		{
			VXL_ASSERT(m_hasLayout, "MeshBufferMem missing Layout");

			dirtyVertices = true;

			std::fill(vertices.begin(), vertices.end(), 0);
		}

		void set(Type* arr, uint32_t size)
		{
			VXL_ASSERT(m_hasLayout, "MeshBufferMem missing Layout");

			// Flags
			dirtyVertices = true;
			resizedVertices = size != (uint32_t)vertices.size();
			// Edit Vertices
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();
		}
		void set(const std::vector<Type>& vec)
		{
			VXL_ASSERT(m_hasLayout, "MeshBufferMem missing Layout");

			// Flags
			dirtyVertices = true;
			resizedVertices = (uint32_t)vec.size() != (uint32_t)vertices.size();
			// Edit Vertices
			vertices = vec;
			vertices.shrink_to_fit();
		}

		Type operator[](uint32_t index) const
		{
			return vertices[index];
		}
		Type& operator[](uint32_t index)
		{
			return vertices[index];
		}

		MeshBufferMem<Type>& operator=(std::vector<uint32_t> vec)
		{
			set(vec);
			return *this;
		}

		// Notice, requires vbo to be bound first //
		inline UINT GetDrawCount(void) const
		{
			return m_vbo.GetDrawCount();
		}
		// Notice, requires vbo to be bound first //
		inline UINT GetSize(void) const
		{
			return m_vbo.GetSize();
		}
		// Notice, requires vbo to be bound first //
		inline bool IsEmpty() const
		{
			return m_vbo.IsEmpty() && (uint32_t)vertices.size() == 0u;
		}
	};

	// Contains EBO + vector for indices
	class MeshBufferIndices
	{
	private:
		EBO m_ebo;
		BufferUsage m_bindMode = BufferUsage::STATIC_DRAW;

		std::vector<uint32_t> indices;
		bool dirtyVertices = false;
		bool resizedVertices = false;
	public:

		// Only applies changes on calling the "Set" function
		void setBindMode(BufferUsage usage)
		{
			m_bindMode = usage;
		}

		std::vector<uint32_t>& getIndices(void)
		{
			return indices;
		}
		uint32_t getIndexCount(void) const
		{
			return (uint32_t)indices.size();
		}

		void setVerticesDirty(bool sizeHasChanged)
		{
			dirtyVertices = true;
			resizedVertices = sizeHasChanged;
		}

		void bind(void)
		{
			if (dirtyVertices)
			{
				if(resizedVertices || m_ebo.IsEmpty())
					m_ebo.SetIndices(indices, m_bindMode);
				else
					m_ebo.UpdateIndices(indices.data(), 0);
			}

			m_ebo.Bind();

			dirtyVertices = false;
			resizedVertices = false;
		}

		void set(uint32_t* arr, uint32_t size)
		{
			// Flags
			dirtyVertices = true;
			resizedVertices = size != (uint32_t)indices.size();
			// Edit Indices
			indices = std::vector<uint32_t>(arr, arr + size);
			indices.shrink_to_fit();
		}
		void set(const std::vector<uint32_t>& vec)
		{
			// Flags
			dirtyVertices = true;
			resizedVertices = (uint32_t)vec.size() != (uint32_t)indices.size();
			// Edit Indices
			indices = vec;
		}

		uint32_t operator[](uint32_t index) const
		{
			return indices[index];
		}
		uint32_t& operator[](uint32_t index)
		{
			return indices[index];
		}

		MeshBufferIndices& operator=(std::vector<uint32_t> vec)
		{
			set(vec);
			return *this;
		}

		// Requires EBO to be bound first //
		inline UINT GetDrawCount(void) const
		{
			return m_ebo.GetDrawCount();
		}
		// Requires EBO to be bound first //
		inline UINT GetSize(void) const
		{
			return m_ebo.GetSize();
		}
		// Requires EBO to be bound first //
		inline bool IsEmpty()
		{
			return m_ebo.IsEmpty() && (uint32_t)indices.size() == 0u;
		}
	};

	class MeshBufferInstancing
	{
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

		void bind(void) const
		{
			m_vbo.Bind();
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

