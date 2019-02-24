// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Debug.h"

#include "../opengl/Texture.h"
#include "../math/Color.h"

namespace Vxl
{
	void Debug::DrawLine(
		const Vector3& P1, const Vector3& P2,
		const Color3F& C1, const Color3F& C2
	)
	{
		if (m_lines_vertexIndex >= m_lines_vertices.size())
		{
			// Increase vector size
			m_lines_vertices.resize(m_lines_vertices.size() + 600);
			m_lines_resizeDirty = true;
		}

		m_lines_vertices[m_lines_vertexIndex + 0] = P1.x;
		m_lines_vertices[m_lines_vertexIndex + 1] = P1.y;
		m_lines_vertices[m_lines_vertexIndex + 2] = P1.z;
		m_lines_vertices[m_lines_vertexIndex + 3] = C1.r;
		m_lines_vertices[m_lines_vertexIndex + 4] = C1.g;
		m_lines_vertices[m_lines_vertexIndex + 5] = C1.b;

		m_lines_vertices[m_lines_vertexIndex + 6] = P2.x;
		m_lines_vertices[m_lines_vertexIndex + 7] = P2.y;
		m_lines_vertices[m_lines_vertexIndex + 8] = P2.z;
		m_lines_vertices[m_lines_vertexIndex + 9] = C2.r;
		m_lines_vertices[m_lines_vertexIndex + 10] = C2.g;
		m_lines_vertices[m_lines_vertexIndex + 11] = C2.b;

		m_lines_vertexIndex += 12;
	}
	void Debug::DrawAABB(
		const Vector3& Min, const Vector3& Max, const Vector3& OffsetAll,
		const Color3F& C
	)
	{
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Max.x, Min.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Min.x, Min.y, Max.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Max.x, Min.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Min.x, Min.y, Max.z), C, C);

		DrawLine(OffsetAll + Vector3(Min.x, Max.y, Min.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Max.y, Min.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Max.y, Max.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Max.y, Max.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), C, C);

		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Min.x, Max.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Max.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Min.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Max.x, Max.y, Max.z), C, C);
	}

	void Debug::UpdateStart()
	{
		// reset index flag
		m_lines_vertexIndex = 0;
	}
	void Debug::RenderLines()
	{
		// No VAO allowed
		glUtil::unbindVAO();

		// Resize VBO if vector resized
		if (m_lines_resizeDirty)
		{
			// Resize VBO
			m_lines->SetVertices(m_lines_vertices, BufferBind_Mode::DYNAMIC);
			m_lines_resizeDirty = false;
		}
		// reset index flag
		m_lines_vertexIndex = 0;

		// Set VBO To lines
		m_lines->Bind();
		// Update Vertices
		m_lines->UpdateVertices(&m_lines_vertices[0]);
		// Draw Lines
		m_lines->Draw(Draw_Type::LINES);
	}

	void Debug::CreateDebugTextures()
	{
		std::vector<Color3F> pixels;
		pixels.reserve(16);

		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));

		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);

		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));

		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);
		pixels.push_back(Color3F(0, 0, 0));
		pixels.push_back(Color3F::PURPLE);

		m_null_texture = Texture::Create("null", pixels, 4, Wrap_Mode::CLAMP_STRETCH, Filter_Mode_Min::NEAREST, Filter_Mode_Mag::NEAREST);
	}
	void Debug::BindNullTexture()
	{
		assert(m_null_texture);
		m_null_texture->Bind();
	}
}