// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"
#include <Windows.h>

#include "../utilities/logger.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace Vxl
{
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
			delete[] m_array;
			m_array = nullptr;
			m_length = 0;
		}

	};

	template<typename Type>
	class MeshBuffer : public MeshBufferBase
	{
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

		void bind() override
		{
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
	};

	class MeshBufferIndices : public MeshBufferBase
	{
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
	};


	class Mesh
	{
	private:
		GLuint  m_VAO;
		GLuint* m_VBOs;

		Draw_Type m_type = Draw_Type::TRIANGLES;
		Draw_Mode m_mode = Draw_Mode::ARRAY;
		GLuint	  m_drawCount = 0;

		void (Mesh::*Draw_Function)(void) = &Mesh::DrawArray;

		void DrawArray();
		void DrawIndexed();

		void UpdateDrawInfo();

	public:
		Mesh();
		virtual ~Mesh();

		MeshBuffer<Vector3>	m_positions;
		MeshBuffer<Vector2>	m_uvs;
		MeshBuffer<Vector3>	m_normals;
		MeshBufferIndices	m_indices;

		inline void			SetDrawType(Draw_Type type)
		{
			m_type = type;
		}

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

		void Bind();
		void Draw();
	};
}

