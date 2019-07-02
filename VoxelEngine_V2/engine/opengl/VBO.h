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
		GLuint			m_VBO = -1;
		GLuint			m_TypeSize = 0; // ex: Float = 4
		GLuint			m_DrawCount = 0; // Values to be read for drawing
		GLuint			m_Size = 0; // Bytes of space used
		bool			m_empty = true;
		BufferLayout	m_layout;
		BufferBind_Mode m_bindMode;
		
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
			VXL_ASSERT(size + offset <= m_Size, "VBO: Size + Offset too large for updating vertices");
			glUtil::bindVBOSubData(m_VBO, offset, size, (GLvoid*)_arr);
		}

		inline void SetLayout(const BufferLayout& layout)
		{
			m_layout = layout;
		}

		inline GLuint GetVBO(void) const
		{
			return m_VBO;
		}
		inline GLuint GetDrawCount(void) const
		{
			return m_DrawCount;
		}
		inline GLuint GetSize(void) const
		{
			return m_Size;
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

