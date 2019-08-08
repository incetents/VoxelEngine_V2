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
	public:

		// Only applies changes on calling the "Set" function
		void setBindMode(BufferUsage usage)
		{
			m_bindMode = usage;
		}

		virtual void bind(void) const
		{
			m_vbo.Bind();
		}

		virtual void setLayout(const BufferLayout& layout)
		{
			m_vbo.SetLayout(layout);
		}

		virtual void set(Type* arr, uint32_t size)
		{
			m_vbo.SetVertices(arr, size, m_bindMode);
		}
		virtual void set(const std::vector<Type>& vec)
		{
			m_vbo.SetVertices(vec.data(), (uint32_t)vec.size(), m_bindMode);
		}

		virtual void update(Type* arr)
		{
			m_vbo.UpdateVertices(arr, 0);
		}
		virtual void update(Type* arr, int offset, int size)
		{
			m_vbo.UpdateVertices(arr, offset, size);
		}

		virtual void update(const std::vector<Type>& vec)
		{
			m_vbo.UpdateVertices(vec.data(), 0);
		}
		virtual void update(const std::vector<Type>& vec, int offset, int size)
		{
			m_vbo.UpdateVertices(vec.data(), offset, size);
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

	// Contains VBO + vector for vertices
	template<typename Type>
	class MeshBufferMem : public MeshBuffer<Type>
	{
	public:
		std::vector<Type> vertices;
		bool dirtyVertices = false;
	public:
		 
		void bind(void) const override
		{
			VXL_ASSERT(!dirtyVertices, "MeshBufferMem Vertices needs to be reloaded");

			MeshBuffer<Type>::m_vbo.Bind();
		}

		// Ref of vertices, non-editable
		const std::vector<Type>& getVertices(void)
		{
			return vertices;
		}
		// Get ref of vertices for editing, must call reload to update changes
		std::vector<Type>& getVerticesEditing(void)
		{
			dirtyVertices = true;
			return vertices;
		}

		// Re-send stored vertices to VBO
		void reload(bool SizeHasChanged)
		{
			dirtyVertices = false;

			if (SizeHasChanged)
				MeshBuffer<Type>::m_vbo.SetVertices(vertices.data(), (uint32_t)vertices.size(), MeshBuffer<Type>::m_bindMode);
			else
				MeshBuffer<Type>::m_vbo.UpdateVertices(vertices.data(), 0);
		}

		// Set all vertices to zeroes
		void clear()
		{
			// Nothing to clear
			if (vertices.size() == 0)
				return;

			std::fill(vertices.begin(), vertices.end(), 0);
			reload(false);
		}

		void set(Type* arr, uint32_t size) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type>::m_vbo.SetVertices(arr, size, MeshBuffer<Type>::m_bindMode);
		}
		void set(const std::vector<Type>& vec) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type>::m_vbo.SetVertices(vec.data(), (uint32_t)vec.size(), MeshBuffer<Type>::m_bindMode);
		}

		void update(Type* arr) override
		{
			vertices = std::vector<Type>(arr, arr + vertices.size());
			vertices.shrink_to_fit();
		
			MeshBuffer<Type>::m_vbo.UpdateVertices(arr, 0);
		}
		void update(Type* arr, int offset, int size) override
		{
			vertices = std::vector<Type>(arr, arr + size);
			vertices.shrink_to_fit();

			MeshBuffer<Type>::m_vbo.UpdateVertices(arr, offset, size);
		}

		void update(const std::vector<Type>& vec) override
		{
			vertices = vec;
			vertices.shrink_to_fit();
		
			MeshBuffer<Type>::m_vbo.UpdateVertices(vec.data(), 0);
		}
		void update(const std::vector<Type>& vec, int offset, int size) override
		{
			vertices = vec;
			vertices.shrink_to_fit();

			MeshBuffer<Type>::m_vbo.UpdateVertices(vec.data(), offset, size);
		}

	};

	// Contains EBO + vector for indices
	class MeshBufferIndices
	{
	private:
		EBO m_ebo;
		std::vector<uint32_t> indices;
		bool dirtyIndices = false;
		BufferUsage m_bindMode = BufferUsage::STATIC_DRAW;
	public:

		// Only applies changes on calling the "Set" function
		void setBindMode(BufferUsage usage)
		{
			m_bindMode = usage;
		}

		void bind(void) const
		{
			VXL_ASSERT(!dirtyIndices, "MeshBuffer Indices needs to be reloaded");

			m_ebo.Bind();
		}
		
		// Ref of indices, non-editable
		const std::vector<uint32_t>& getIndices(void)
		{
			return indices;
		}
		// Get ref of vertices for editing, must call reload to update changes
		std::vector<uint32_t>& getIndicesEditing(void)
		{
			dirtyIndices = true;
			return indices;
		}

		// Re-send stored indices to EBO
		void reload(bool SizeHasChanged)
		{
			dirtyIndices = false;

			if (SizeHasChanged)
				m_ebo.SetIndices(indices, m_bindMode);
			else
				m_ebo.UpdateIndices(indices.data(), 0);
		}

		void set(uint32_t* arr, uint32_t size)
		{
			indices = std::vector<uint32_t>(arr, arr + size);
			indices.shrink_to_fit();

			m_ebo.SetIndices(arr, size, m_bindMode);
		}
		void set(const std::vector<uint32_t>& vec)
		{
			indices = vec;
			indices.shrink_to_fit();

			m_ebo.SetIndices(vec, m_bindMode);
		}

		void update(uint32_t* arr)
		{
			indices = std::vector<uint32_t>(arr, arr + indices.size());
			indices.shrink_to_fit();
		
			m_ebo.UpdateIndices(arr, 0);
		}
		void update(uint32_t* arr, int offset, int size)
		{
			indices = std::vector<uint32_t>(arr, arr + size);
			indices.shrink_to_fit();

			m_ebo.UpdateIndices(arr, offset, size);
		}

		void update(std::vector<uint32_t>& vec)
		{
			indices = vec;
			indices.shrink_to_fit();
		
			m_ebo.UpdateIndices(vec.data(), 0);
		}
		void update(std::vector<uint32_t>& vec, int offset, int size)
		{
			indices = vec;
			indices.shrink_to_fit();

			m_ebo.UpdateIndices(vec.data(), offset, size);
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

