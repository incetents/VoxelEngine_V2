// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Debug.h"

#include "../math/Color.h"
#include "../modules/Entity.h"
#include "../rendering/Primitives.h"
#include "../rendering/Mesh.h"
#include "../textures/Texture2D.h"
#include "../utilities/Asset.h"

namespace Vxl
{
	void Debug::DrawLine(
		const Vector3& P1, const Vector3& P2,
		float Width,
		const Color3F& C1, const Color3F& C2
	)
	{
		m_worldLines->addLine(P1, P2, Width, C1, C2);
	}
	void Debug::DrawLineNoDepth(
		const Vector3& P1, const Vector3& P2,
		float Width,
		const Color3F& C1, const Color3F& C2
	)
	{
		m_worldLinesNoDepth->addLine(P1, P2, Width, C1, C2);
	}
	void Debug::DrawLineAABB(
		const Vector3& Min, const Vector3& Max,
		const Vector3& OffsetAll,
		float Width,
		const Color3F& C
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
	void Debug::DrawLineOBB(
		const Entity& entity,
		const Vector3& OffsetAll,
		float Width,
		const Color3F& C
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
	void Debug::DrawLineSquare(
		const Vector3& position,
		const Vector3& up,
		const Vector3& right,
		float Width,
		const Color3F& C
	)
	{
		DrawLine(position + up - right, position + up + right, Width, C, C);
		DrawLine(position + up + right, position - up + right, Width, C, C);
		DrawLine(position - up + right, position - up - right, Width, C, C);
		DrawLine(position - up - right, position + up - right, Width, C, C);
	}

	void Debug::DrawCube(
		const Vector3& position,
		const Vector3& scale,
		const Color3F& C
	)
	{
		ColoredModel _object;
		_object.m_model = Matrix4x4::GetScale(scale);
		_object.m_model.OverrideCenter(position);
		_object.m_color = C;

		m_cubes.push_back(_object);
	}
	void Debug::DrawCube(
		const Matrix4x4& model,
		const Color3F& C
	)
	{
		ColoredModel _object;
		_object.m_model = model;
		_object.m_color = C;

		m_cubes.push_back(_object);
	}
	void Debug::DrawWireframeSphere(
		const Vector3& position,
		const Vector3& scale,
		const Color3F& C
	)
	{
		ColoredModel _object;
		_object.m_model = Matrix4x4::GetScale(scale);
		_object.m_model.OverrideCenter(position);
		_object.m_color = C;

		m_wireframeSpheres.push_back(_object);
	}
	void Debug::DrawWireframeSphere(
		const Matrix4x4& model,
		const Color3F& C
	)
	{
		ColoredModel _object;
		_object.m_model = model;
		_object.m_color = C;

		m_wireframeSpheres.push_back(_object);
	}

	void Debug::DrawLineScreenSpace(
		const Vector2& P1, const Vector2& P2,
		float Width,
		const Color3F& C1, const Color3F& C2
	){
		m_screenLines->addLine(P1, P2, Width, C1, C2);
	}
	void Debug::DrawLineSquareScreenSpace(
		const Vector2& P, const Vector2& Size,
		float LineWidth,
		const Color3F& Color
	){
		float halfW = Size.x * 0.5f;
		float halfH = Size.y * 0.5f;
		m_screenLines->addLine(P + vec2(+halfW, +halfH), P + vec2(-halfW, +halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(-halfW, +halfH), P + vec2(-halfW, -halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(-halfW, -halfH), P + vec2(+halfW, -halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(+halfW, -halfH), P + vec2(+halfW, +halfH), LineWidth, Color, Color);
	}

	void Debug::RenderWorldLines()
	{
		m_worldLines->bind();
		m_worldLines->draw();
	}

	void Debug::RenderWorldLinesNoDepth()
	{
		m_worldLinesNoDepth->bind();
		m_worldLinesNoDepth->draw();
	}

	void Debug::RenderScreenLines()
	{
		m_screenLines->bind();
		m_screenLines->draw();
	}

	void Debug::End()
	{
		// reset ColoredModels
		m_wireframeSpheres.clear();
		m_cubes.clear();
	}
}