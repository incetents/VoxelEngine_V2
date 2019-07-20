// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "VBOLayout.h"

#include "../utilities/Macros.h"

#include <assert.h>
#include <vector>
#include <map>

namespace Vxl
{
	// Vertex Array Object Wrapper
	class VAO
	{
	private:
		VAOID m_VAO = -1;
	public:
		VAO()
		{
			m_VAO = Graphics::VAO::Create();
		}
		~VAO()
		{
			Graphics::VAO::Delete(m_VAO);
		}

		inline uint32_t GetID(void) const
		{
			return m_VAO;
		}

		void bind(void) const
		{
			Graphics::VAO::Bind(m_VAO);
		}
		void unbind(void) const
		{
			Graphics::VAO::Unbind();
		}
	};

	// Vertex Buffer Object Wrapper
	class VBO
	{
	private:
		VBOID			m_VBO = -1;
		uint32_t			m_TypeSize = 0; // ex: Float = 4
		uint32_t			m_DrawCount = 0; // Values to be read for drawing
		uint32_t			m_Size = 0; // Bytes of space used
		bool			m_empty = true;
		BufferLayout	m_layout;
		BufferUsage m_bindMode;
		
		void UpdateDrawCount();

	public:	
		template<typename Type = float>
		VBO()
		{
			m_TypeSize = sizeof(Type);
		}
		template<typename Type = float>
		VBO(Type* _arr, uint32_t _count, BufferUsage _mode = BufferUsage::STATIC_DRAW)
		{
			m_TypeSize = sizeof(Type);
			SetVertices<Type>(_arr, _count, _mode);
		}
		template<typename Type = float>
		VBO(std::vector<Type> _arr, BufferUsage _mode = BufferUsage::STATIC_DRAW)
		{
			m_TypeSize = sizeof(Type);
			SetVertices<Type>(_arr, _mode);
		}

		~VBO()
		{
			if (m_VBO != -1)
				Graphics::VBO::Delete(m_VBO);
		}

		// If Type is not a float, it must be an object containing floats
		template<typename Type = float>
		void SetVertices(Type* _arr, uint32_t _count, BufferUsage _mode)
		{
			if (_count == 0)
				return;

			if (m_VBO == -1)
				m_VBO = Graphics::VBO::Create();

			m_empty = false;
			m_Size = _count * sizeof(Type);
			m_bindMode = _mode;

			Graphics::VBO::Bind(m_VBO);
			Graphics::VBO::BindData(m_Size, (void*)_arr, _mode);

			UpdateDrawCount();
		}
		template<typename Type = float>
		void SetVertices(std::vector<Type> _arr, BufferUsage _mode)
		{
			SetVertices(&_arr[0], (uint32_t)_arr.size(), _mode);
		}

		template<typename Type = float>
		void UpdateVertices(Type* _arr, int offset)
		{
			Graphics::VBO::Bind(m_VBO);
			Graphics::VBO::BindSubData(offset, m_Size, (void*)_arr);
		}

		template<typename Type = float>
		void UpdateVertices(Type* _arr, int offset, uint32_t size)
		{
			VXL_ASSERT(size + offset <= m_Size, "VBO: Size + Offset too large for updating vertices");

			Graphics::VBO::Bind(m_VBO);
			Graphics::VBO::BindSubData(offset, size, (void*)_arr);
		}

		inline void SetLayout(const BufferLayout& layout)
		{
			m_layout = layout;
		}

		inline uint32_t GetVBO(void) const
		{
			return m_VBO;
		}
		inline uint32_t GetDrawCount(void) const
		{
			return m_DrawCount;
		}
		inline uint32_t GetSize(void) const
		{
			return m_Size;
		}
		inline bool IsEmpty(void) const
		{
			return m_empty;
		}
		
		inline BufferUsage GetBindMode(void) const
		{
			return m_bindMode;
		}

		void Bind();
	};

	// Element Buffer Object
	class EBO
	{
	private:
		EBOID m_EBO = -1;
		uint32_t m_Size = 0;
		uint32_t m_DrawCount = 0;
		bool m_empty = true;

		BufferUsage m_bindMode;
	public:
		EBO() {}
		EBO(uint32_t* _arr, uint32_t _count, BufferUsage _mode = BufferUsage::STATIC_DRAW)
		{
			SetIndices(_arr, _count, _mode);
		}
		~EBO()
		{
			if(m_EBO != -1)
				Graphics::EBO::Delete(m_EBO);
		}

		void SetIndices(uint32_t* _arr, uint32_t _count, BufferUsage _mode = BufferUsage::STATIC_DRAW);
		void SetIndices(std::vector<uint32_t> _arr, BufferUsage _mode = BufferUsage::STATIC_DRAW);

		void UpdateIndices(uint32_t* _arr, int offset = 0)
		{
			Graphics::EBO::Bind(m_EBO);
			Graphics::EBO::BindSubData(offset, m_Size, _arr);
		}

		inline uint32_t GetVBO(void) const
		{
			return m_EBO;
		}
		inline uint32_t GetSize(void) const
		{
			return m_Size;
		}
		inline uint32_t GetDrawCount(void) const
		{
			return m_DrawCount;
		}
		inline bool IsEmpty(void) const
		{
			return m_empty;
		}

		inline BufferUsage GetBindMode(void) const
		{
			return m_bindMode;
		}

		void Bind();
	};
}

