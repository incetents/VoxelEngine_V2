// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"

#include "VBO.h"

namespace Vxl
{
	// MeshBuffer
	template<typename Type>
	class MeshBuffer
	{
		DISALLOW_COPY_AND_ASSIGN(MeshBuffer);
	private:
		VBO			m_vbo;
		BufferUsage m_bindMode;
		uint32_t	m_storedVertexSize = 0;
	public:

		// Vertices to modify
		std::vector<Type> vertices;

		// Utility
		MeshBuffer<Type>& operator=(const std::vector<Type>& _vector)
		{
			vertices = _vector;
			return *this;
		}
		bool isEmpty(void) const
		{
			return !vertices.size();
		}
		uint32_t getDrawCount(void) const
		{
			return m_vbo.GetDrawCount();
		}
		uint32_t size(void) const
		{
			return (uint32_t)vertices.size();
		}

		// System
		void setLayout(const BufferLayout& layout)
		{
			m_vbo.SetLayout(layout);
		}
		void bind()
		{
			uint32_t verticesCount = (uint32_t)vertices.size();

			// VBO does nothing and receives nothing, therefore don't use this buffer
			if (verticesCount == 0 && m_storedVertexSize == 0)
				return;

			// Check if size has changed
			if (m_storedVertexSize != verticesCount)
			{
				m_vbo.SetVertices(vertices.data(), (uint32_t)vertices.size(), m_bindMode);
			}
			// Size hasn't changed, just update vertices
			else
			{
				m_vbo.UpdateVertices(vertices.data(), 0);
			}
			m_storedVertexSize = verticesCount;

			m_vbo.bind();
		}

		//
		MeshBuffer(const BufferLayout& layout, BufferUsage bindMode)
		{
			m_bindMode = bindMode;
			m_vbo.SetLayout(layout);
		}
	};

	class MeshBufferIndices
	{
		DISALLOW_COPY_AND_ASSIGN(MeshBufferIndices);
	private:
		EBO			m_ebo;
		BufferUsage m_bindMode;
		uint32_t	m_storedVertexSize = 0;
	public:

		// Vertices to modify
		std::vector<uint32_t> vertices;

		// Utility
		MeshBufferIndices& operator=(const std::vector<uint32_t>& _vector)
		{
			vertices = _vector;
			return *this;
		}
		uint32_t getDrawCount(void) const
		{
			return m_ebo.GetDrawCount();
		}
		uint32_t size(void) const
		{
			return (uint32_t)vertices.size();
		}

		// System
		void bind()
		{
			uint32_t verticesCount = (uint32_t)vertices.size();

			// VBO does nothing and receives nothing
			if (verticesCount == 0 && m_storedVertexSize == 0)
				return;

			// Check if size has changed
			if (m_storedVertexSize != verticesCount)
			{
				m_ebo.SetIndices(vertices.data(), (uint32_t)vertices.size(), m_bindMode);
			}
			// Size hasn't changed, just update vertices
			else
			{
				m_ebo.UpdateIndices(vertices.data(), 0);
			}
			m_storedVertexSize = verticesCount;

			m_ebo.bind();
		}

		//
		MeshBufferIndices(BufferUsage bindMode)
		{
			m_bindMode = bindMode;
		}
	};

}

