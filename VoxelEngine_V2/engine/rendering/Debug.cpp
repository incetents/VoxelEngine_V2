// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Debug.h"

#include "../math/Color.h"
#include "../math/Collision.h"
#include "../modules/Entity.h"
#include "../rendering/Primitives.h"
#include "../rendering/Mesh.h"
#include "../textures/Texture2D.h"
#include "../utilities/Asset.h"
#include "../utilities/Time.h"

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
	void Debug::DrawLineScreenSpace(
		const Vector2& P1, const Vector2& P2,
		float Width,
		const Color3F& C1, const Color3F& C2
	) {
		m_screenLines->addLine(P1, P2, Width, C1, C2);
	}
	void Debug::DrawLineSquareScreenSpace(
		const Vector2& P, const Vector2& Size,
		float LineWidth,
		const Color3F& Color
	) {
		float halfW = Size.x * 0.5f;
		float halfH = Size.y * 0.5f;
		m_screenLines->addLine(P + vec2(+halfW, +halfH), P + vec2(-halfW, +halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(-halfW, +halfH), P + vec2(-halfW, -halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(-halfW, -halfH), P + vec2(+halfW, -halfH), LineWidth, Color, Color);
		m_screenLines->addLine(P + vec2(+halfW, -halfH), P + vec2(+halfW, +halfH), LineWidth, Color, Color);
	}
	void Debug::DrawLineCube(
		const Vector3& position,
		float width,
		const Vector3& scale,
		const Vector3& eulerRotation,
		const Color3F& C
	)
	{
		Object _object;
		Quaternion rotation = Quaternion::ToQuaternion_ZYX(ToRadians(eulerRotation.x), ToRadians(eulerRotation.y), ToRadians(eulerRotation.z));
		_object.model = Matrix4x4(rotation.GetMatrix3x3() * Matrix3x3::GetScale(scale), position);
		_object.color = C;
		_object.width = width;

		m_cubesLines.push_back(_object);
	}
	void Debug::DrawLineSphere(
		const Vector3& position,
		float width,
		const Vector3& scale,
		const Color3F& C
	)
	{
		Object _object;
		_object.model = Matrix4x4(Matrix3x3::GetScale(scale), position);
		_object.color = C;
		_object.width = width;

		m_spheresLines.push_back(_object);
	}

	void Debug::DrawLineAABB(
		const Vector3& Min, const Vector3& Max,
		float width,
		const Color3F& C
	)
	{
		Object _object;
		_object.model = Matrix4x4(Matrix3x3::GetScale(Max - Min), (Min + Max) * 0.5f);
		_object.color = C;
		_object.width = width;

		m_cubesLines.push_back(_object);
	}
	void Debug::DrawLineAABB(
		const AABB& aabb,
		float width,
		const Color3F& C
	)
	{
		Object _object;
		_object.model = Matrix4x4(Matrix3x3::GetScale(aabb.max - aabb.min), (aabb.max + aabb.min) * 0.5f);
		_object.color = C;
		_object.width = width;

		m_cubesLines.push_back(_object);
	}
	void Debug::DrawLineOBB(
		const OBB& obb,
		float Width,
		const Color3F& C
	)
	{
		Object _object;
		Matrix3x3 rotScale = Matrix3x3::createFromColumns(obb.right, obb.up, obb.forward);
		_object.model = Matrix4x4(rotScale, obb.position);
		_object.color = C;
		_object.width = Width;

		m_cubesLines.push_back(_object);
	}
	void Debug::DrawLineArrow(
		const Vector3& p1, const Vector3& p2,
		float Width,
		float ArrowTipSize,
		const Color3F& C
	)
	{
		// Base line
		DrawLine(p1, p2, Width, C, C);

		Object _object;

		// Non rotation Forward is (0,0,1)
		Vector3 direction = (p2 - p1);

		// Y Rotation
		Vector2 directionXZ = Vector2(direction.x, direction.z);
		float YrotationRad = Vector2::GetAngleRadians(Vector2(directionXZ).NormalizeAccurate(), Vector2(0, 1));
		if (direction.x > 0)
			YrotationRad = -YrotationRad;

		// X Rotation
		float XrotationRad = -Vector3::GetAngleRadians(direction.NormalizeAccurate(), Vector3::UP) + HALF_PI;//

		// Rotation Result
		Quaternion QY = Quaternion::AngleAxis(YrotationRad, Vector3::UP);
		Quaternion QX = Quaternion::AngleAxis(XrotationRad, Vector3::RIGHT);

		_object.model = Matrix4x4((QY * QX).GetMatrix3x3() * Matrix3x3::GetScale(ArrowTipSize), p2);
		_object.color = C;
		_object.width = Width;

		m_arrowLines.push_back(_object);
	}

	void Debug::DrawCube(
		const Vector3& position,
		const Vector3& scale,
		const Vector3& eulerRotation,
		const Color3F& C
	)
	{
		Object _object;
		Quaternion rotation = Quaternion::ToQuaternion_ZYX(ToRadians(eulerRotation.x), ToRadians(eulerRotation.y), ToRadians(eulerRotation.z));
		_object.model = Matrix4x4(rotation.GetMatrix3x3() * Matrix3x3::GetScale(scale), position);
		_object.color = C;

		m_cubes.push_back(_object);
	}
	void Debug::DrawCube(
		const Matrix4x4& model,
		const Color3F& C
	)
	{
		Object _object;
		_object.model = model;
		_object.color = C;

		m_cubes.push_back(_object);
	}
	void Debug::DrawSphere(
		const Vector3& position,
		const Vector3& scale,
		const Vector3& eulerRotation,
		const Color3F& C
	)
	{
		Object _object;
		Quaternion rotation = Quaternion::ToQuaternion_ZYX(ToRadians(eulerRotation.x), ToRadians(eulerRotation.y), ToRadians(eulerRotation.z));
		_object.model = Matrix4x4(rotation.GetMatrix3x3() * Matrix3x3::GetScale(scale), position);
		_object.color = C;

		m_spheres.push_back(_object);
	}
	void Debug::DrawSphere(
		const Matrix4x4& model,
		const Color3F& C
	)
	{
		Object _object;
		_object.model = model;
		_object.color = C;

		m_spheres.push_back(_object);
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
		m_worldLines->resetIndex();
		m_worldLinesNoDepth->resetIndex();
		m_screenLines->resetIndex();

		m_spheres.clear();
		m_cubes.clear();

		m_spheresLines.clear();
		m_cubesLines.clear();

		m_arrowLines.clear();
	}
}