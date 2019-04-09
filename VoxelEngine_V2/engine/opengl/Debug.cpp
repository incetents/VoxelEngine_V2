// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Debug.h"

#include "../opengl/Texture.h"
#include "../math/Color.h"
#include "../utilities/Loader.h"
#include "../modules/Entity.h"

namespace Vxl
{
	const UINT Debug::m_lines_vertexIncrementAmount = 800;

	void Debug::DrawLine(
		const Vector3& P1, const Vector3& P2,
		float Width,
		const Color4F& C1, const Color4F& C2
	)
	{
		if (m_lines_vertexIndex >= m_lines_vertices.size())
		{
			// Increase vector size
			m_lines_vertices.resize(m_lines_vertices.size() + m_lines_vertexIncrementAmount);
			m_lines_resizeDirty = true;
		}

		m_lines_vertices[m_lines_vertexIndex + 0] = P1.x;
		m_lines_vertices[m_lines_vertexIndex + 1] = P1.y;
		m_lines_vertices[m_lines_vertexIndex + 2] = P1.z;
		m_lines_vertices[m_lines_vertexIndex + 3] = C1.r;
		m_lines_vertices[m_lines_vertexIndex + 4] = C1.g;
		m_lines_vertices[m_lines_vertexIndex + 5] = C1.b;
		m_lines_vertices[m_lines_vertexIndex + 6] = C1.a;

		m_lines_vertices[m_lines_vertexIndex + 7] = Width;

		m_lines_vertices[m_lines_vertexIndex + 8] = P2.x;
		m_lines_vertices[m_lines_vertexIndex + 9] = P2.y;
		m_lines_vertices[m_lines_vertexIndex + 10] = P2.z;
		m_lines_vertices[m_lines_vertexIndex + 11] = C2.r;
		m_lines_vertices[m_lines_vertexIndex + 12] = C2.g;
		m_lines_vertices[m_lines_vertexIndex + 13] = C2.b;
		m_lines_vertices[m_lines_vertexIndex + 14] = C2.a;

		m_lines_vertices[m_lines_vertexIndex + 15] = Width;

		m_lines_vertexIndex += 16;
	}
	void Debug::DrawAABB(
		const Vector3& Min, const Vector3& Max,
		const Vector3& OffsetAll,
		float Width,
		const Color4F& C
	)
	{
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Max.x, Min.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Min.x, Min.y, Max.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Max.x, Min.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Min.x, Min.y, Max.z), Width, C, C);

		DrawLine(OffsetAll + Vector3(Min.x, Max.y, Min.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Max.y, Min.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Max.y, Max.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Max.y, Max.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), Width, C, C);

		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Min.z), OffsetAll + Vector3(Min.x, Max.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Min.x, Min.y, Max.z), OffsetAll + Vector3(Min.x, Max.y, Max.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Min.z), OffsetAll + Vector3(Max.x, Max.y, Min.z), Width, C, C);
		DrawLine(OffsetAll + Vector3(Max.x, Min.y, Max.z), OffsetAll + Vector3(Max.x, Max.y, Max.z), Width, C, C);
	}
	void Debug::DrawOBB(
		const Entity& entity,
		const Vector3& OffsetAll,
		float Width,
		const Color4F& C
	)
	{
		auto OBB = entity.GetOBB();
		// Bot
		DrawLine(OffsetAll + OBB[0], OffsetAll + OBB[1], Width, C, C);
		DrawLine(OffsetAll + OBB[1], OffsetAll + OBB[5], Width, C, C);
		DrawLine(OffsetAll + OBB[5], OffsetAll + OBB[4], Width, C, C);
		DrawLine(OffsetAll + OBB[4], OffsetAll + OBB[0], Width, C, C);

		// Top
		DrawLine(OffsetAll + OBB[2], OffsetAll + OBB[3], Width, C, C);
		DrawLine(OffsetAll + OBB[3], OffsetAll + OBB[7], Width, C, C);
		DrawLine(OffsetAll + OBB[7], OffsetAll + OBB[6], Width, C, C);
		DrawLine(OffsetAll + OBB[6], OffsetAll + OBB[2], Width, C, C);

		// Mid
		DrawLine(OffsetAll + OBB[0], OffsetAll + OBB[2], Width, C, C);
		DrawLine(OffsetAll + OBB[1], OffsetAll + OBB[3], Width, C, C);
		DrawLine(OffsetAll + OBB[4], OffsetAll + OBB[6], Width, C, C);
		DrawLine(OffsetAll + OBB[5], OffsetAll + OBB[7], Width, C, C);
	}
	void Debug::DrawSquare(
		const Vector3& position,
		const Vector3& up,
		const Vector3& right,
		float Width,
		const Color4F& C
	)
	{
		DrawLine(position + up - right, position + up + right, Width, C, C);
		DrawLine(position + up + right, position - up + right, Width, C, C);
		DrawLine(position - up + right, position - up - right, Width, C, C);
		DrawLine(position - up - right, position + up - right, Width, C, C);
	}

	void Debug::UpdateStart()
	{
		// reset index flag
		m_lines_vertexIndex = 0;
		std::fill(m_lines_vertices.begin(), m_lines_vertices.end(), 0);
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
		glUtil::depthMask(false);
		m_lines->Draw(Draw_Type::LINES);
		glUtil::depthMask(true);
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

		m_null_texture = Texture::CreateCustom("null", pixels, 4, true, Wrap_Mode::CLAMP_STRETCH, Filter_Min::NEAREST, Filter_Mag::NEAREST);
		assert(m_null_texture);
	}
}