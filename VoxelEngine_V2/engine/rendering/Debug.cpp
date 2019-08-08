// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Debug.h"

#include "../textures/Texture.h"
#include "../math/Color.h"
#include "../utilities/Loader.h"
#include "../modules/Entity.h"
#include "../rendering/Geometry.h"
#include "../rendering/Mesh.h"

namespace Vxl
{
	const UINT Debug::VertexIncrementAmount = 16;

	void Debug::DrawLine(
		const Vector3& P1, const Vector3& P2,
		float Width,
		const Color4F& C1, const Color4F& C2
	)
	{
		m_worldLines.AddLine_Vec3(P1, P2, Width, C1, C2);
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

	void Debug::DrawWireframeSphere(
		const Vector3& position,
		const Vector3& scale,
		const Color4F& C
	)
	{
		WireframeSphere WS;
		WS.color = C;
		WS.calcModel(position, scale);

		m_wireframeSpheres.push_back(WS);
	}
	void Debug::DrawWireframeSphere(
		const Matrix4x4& model,
		const Color4F& C
	)
	{
		WireframeSphere WS;
		WS.color = C;
		WS.model = model;

		m_wireframeSpheres.push_back(WS);
	}

	void Debug::DrawScreenSpaceLine(
		const Vector2& P1, const Vector2& P2,
		float Width,
		const Color4F& C1, const Color4F& C2
	){
		m_screenLines.AddLine_Vec2(P1, P2, Width, C1, C2);
	}
	void Debug::DrawScreenSpaceSquare(
		const Vector2& P, const Vector2& Size,
		float LineWidth,
		const Color4F& Color
	){
		float halfW = Size.x * 0.5f;
		float halfH = Size.y * 0.5f;
		m_screenLines.AddLine_Vec2(P + vec2(+halfW, +halfH), P + vec2(-halfW, +halfH), LineWidth, Color, Color);
		m_screenLines.AddLine_Vec2(P + vec2(-halfW, +halfH), P + vec2(-halfW, -halfH), LineWidth, Color, Color);
		m_screenLines.AddLine_Vec2(P + vec2(-halfW, -halfH), P + vec2(+halfW, -halfH), LineWidth, Color, Color);
		m_screenLines.AddLine_Vec2(P + vec2(+halfW, -halfH), P + vec2(+halfW, +halfH), LineWidth, Color, Color);
	}

	void Debug::RenderWorldLines()
	{
		// If vertex index is at start, nothing is being drawn anyways
		if (m_worldLines.m_mesh->m_vertexIndex > 0)
		{
			m_worldLines.m_mesh->m_buffer.reload(m_worldLines.m_mesh->m_resizeDirty);

			m_worldLines.m_mesh->Bind();

			m_worldLines.m_mesh->Draw();
		}
	}

	void Debug::RenderScreenLines()
	{
		// If vertex index is at start, nothing is being drawn anyways
		if (m_screenLines.m_mesh->m_vertexIndex > 0)
		{
			m_screenLines.m_mesh->m_buffer.reload(m_screenLines.m_mesh->m_resizeDirty);
			m_screenLines.m_mesh->Bind();

			m_screenLines.m_mesh->Draw();
		}
	}

	void Debug::End()
	{
		// reset index flag
		m_worldLines.m_mesh->m_vertexIndex = 0;
		m_worldLines.m_mesh->m_buffer.clear();

		m_screenLines.m_mesh->m_vertexIndex = 0;
		m_screenLines.m_mesh->m_buffer.clear();

		// reset wireframe sphere list
		m_wireframeSpheres.clear();
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

		m_null_texture = Texture::CreateCustom("null", pixels, 4, true,
			TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST,
			TextureFormat::RGB8, TextureChannelType::RGB, TexturePixelType::UNSIGNED_BYTE
		);

		VXL_ASSERT(m_null_texture, "Failed to create null texture");
	}
}