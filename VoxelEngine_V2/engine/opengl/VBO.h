// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"

#include <assert.h>
#include <vector>

namespace Vxl
{
	// List of buffer types (non-custom names reflect built-in attribute locations)
	enum class BufferType
	{
		NONE = -1, // Dev use only
		POSITION = 0, VERTEX = 0,
		UV = 1, LINEWIDTH = 1,
		NORMAL = 2,
		COLOR = 3,
		SECOND_COLOR = 4,
		FOG = 5,
		TANGENT = 6,
		BITANGENT = 7, 
		UV1 = 8, InstancingStart = 8, // (instancing)
		UV2 = 9,
		UV3 = 10,
		UV4 = 11,
		UV5 = 12,
		UV6 = 13,
		UV7 = 14,
		UV8 = 15
	};

	// Vertex Array Object Wrapper
	class VAO
	{
	private:
		GLuint m_VAO = -1;
	public:
		VAO()
		{
			glGenVertexArrays(1, &m_VAO);
		}
		~VAO()
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		inline GLuint GetID(void) const
		{
			return m_VAO;
		}

		void bind(void) const
		{
			glBindVertexArray(m_VAO);
		}
		void unbind(void) const
		{
			glBindVertexArray(0);
		}
	};

	// Vertex Buffer Object Wrapper
	class VBO
	{
	private:
		GLuint m_VBO = -1;
		GLuint m_TypeSize = 0; // ex: Float = 4
		GLuint m_Size = 0;
		GLuint m_DrawCount = 0;
		GLsizei m_Stride = 0;
		bool m_empty = true;
		
		BufferBind_Mode m_bindMode;

		// Stride Information Here
		struct StrideHint
		{
			BufferType	m_type = BufferType::VERTEX;
			GLuint		m_valueCount = 0;
			GLuint		m_strideOffset = 0;
		};
		std::vector<StrideHint> m_strideHints;
		UINT m_strideCount = 0;
		inline StrideHint* GetHint(BufferType type)
		{
			for (UINT i = 0; i < m_strideCount; i++)
			{
				if (type == m_strideHints[i].m_type)
					return &m_strideHints[i];
			}
			return nullptr;
		}
		inline void RemoveHint(BufferType type)
		{
			UINT Count = (UINT)m_strideHints.size();
			for (UINT i = 0; i < Count; i++)
			{
				if (m_strideHints[i].m_type == type)
				{
					m_strideHints.erase(m_strideHints.begin() + i);
					return;
				}
			}
		}
		
		void UpdateDrawCount();

	public:	
		template<typename Type = GLfloat>
		VBO()
		{
			m_TypeSize = sizeof(Type);
		}
		template<typename Type = GLfloat>
		VBO(Type* _arr, GLuint _count, BufferBind_Mode _mode = BufferBind_Mode::STATIC)
		{
			m_TypeSize = sizeof(Type);
			SetVertices<Type>(_arr, _count, _mode);
		}
		template<typename Type = GLfloat>
		VBO(std::vector<Type> _arr, BufferBind_Mode _mode = BufferBind_Mode::STATIC)
		{
			m_TypeSize = sizeof(Type);
			SetVertices<Type>(_arr, _mode);
		}

		~VBO()
		{
			RemoveAllHints();

			if (m_VBO != -1)
				glDeleteBuffers(1, &m_VBO);
		}

		// If Type is not a float, it must be an object containing floats
		template<typename Type = GLfloat>
		void SetVertices(Type* _arr, GLuint _count, BufferBind_Mode _mode)
		{
			if (_count == 0)
				return;

			if (m_VBO == -1)
				glGenBuffers(1, &m_VBO);

			m_empty = false;
			m_Size = _count * sizeof(Type);
			m_bindMode = _mode;

			glUtil::bindArray(m_VBO, m_Size, (GLvoid*)_arr, (GLenum)_mode);

			UpdateDrawCount();
		}
		template<typename Type = GLfloat>
		void SetVertices(std::vector<Type> _arr, BufferBind_Mode _mode)
		{
			SetVertices(&_arr[0], (GLuint)_arr.size(), _mode);
		}

		template<typename Type = GLfloat>
		void UpdateVertices(Type* _arr, int offset)
		{
			glUtil::bindVBOSubData(m_VBO, offset, m_Size, (GLvoid*)_arr);
		}

		template<typename Type = GLfloat>
		void UpdateVertices(Type* _arr, int offset, GLuint size)
		{
			assert(size <= m_Size);
			glUtil::bindVBOSubData(m_VBO, offset, size, (GLvoid*)_arr);
		}

		void AddStrideHint(BufferType _type, GLuint _valueCount);
		void RemoveStrideHint(BufferType _type);
		inline void RemoveAllHints()
		{
			m_strideHints.clear();
		}

		inline GLuint GetVBO(void) const
		{
			return m_VBO;
		}
		inline GLuint GetSize(void) const
		{
			return m_Size;
		}
		inline GLuint GetDrawCount(void) const
		{
			return m_DrawCount;
		}
		inline GLsizei GetStride(void) const
		{
			return m_Stride;
		}
		inline bool IsEmpty(void) const
		{
			return m_empty;
		}
		
		inline BufferBind_Mode GetBindMode(void) const
		{
			return m_bindMode;
		}

		void Bind();
		void Draw(Draw_Type _draw);
	};

	// Element Buffer Object
	class EBO
	{
	private:
		GLuint m_EBO = -1;
		GLuint m_Size = 0;
		GLuint m_DrawCount = 0;
		bool m_empty = true;

		BufferBind_Mode m_bindMode;
	public:
		EBO() {}
		EBO(GLuint* _arr, GLuint _count, BufferBind_Mode _mode = BufferBind_Mode::STATIC)
		{
			SetIndices(_arr, _count, _mode);
		}
		~EBO()
		{
			glDeleteBuffers(1, &m_EBO);
		}

		void SetIndices(GLuint* _arr, GLuint _count, BufferBind_Mode _mode = BufferBind_Mode::STATIC);
		void SetIndices(std::vector<GLuint> _arr, BufferBind_Mode _mode = BufferBind_Mode::STATIC);

		void UpdateIndices(GLuint* _arr, int offset = 0)
		{
			glUtil::bindVBOSubData(m_EBO, offset, m_Size, _arr);
		}

		inline GLuint GetVBO(void) const
		{
			return m_EBO;
		}
		inline GLuint GetSize(void) const
		{
			return m_Size;
		}
		inline GLuint GetDrawCount(void) const
		{
			return m_DrawCount;
		}
		inline bool IsEmpty(void) const
		{
			return m_empty;
		}

		inline BufferBind_Mode GetBindMode(void) const
		{
			return m_bindMode;
		}

		void Bind();
		void Draw(Draw_Type _draw);
	};
}

