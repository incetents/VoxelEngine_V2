// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

#include "../modules/Entity.h"
#include "../rendering/Mesh.h"

#include "VBO.h"

#include <vector>
#include <assert.h>

namespace Vxl
{
	class Texture2D;

	struct ColoredModel
	{
		Color4F		m_color;
		Matrix4x4	m_model;
	};

	static class Debug : public Singleton<class Debug>
	{
		friend class RenderManager;

	private:
		// Debug Lines in world space
		LineMesh3D* m_worldLines;
		LineMesh3D* m_worldLinesNoDepth;
		// Debug Lines in screen space
		LineMesh2D* m_screenLines;

		// Debug Wireframe Spheres
		std::vector<ColoredModel> m_wireframeSpheres;
		// Debug Cubes
		std::vector<ColoredModel> m_cubes;

	public:
		void InitGLResources()
		{
			m_worldLines = new LineMesh3D();
			m_worldLinesNoDepth = new LineMesh3D();
			m_screenLines = new LineMesh2D();
		}
		void DestroyGLResources()
		{
			delete m_worldLines;
			delete m_worldLinesNoDepth;
			delete m_screenLines;
		}

		// Line Drawing
		void DrawLine(
			const Vector3& P1, const Vector3& P2,
			float Width,
			const Color3F& C1 = Color3F(1, 1, 1), const Color3F& C2 = Color3F(1, 1, 1)
		);
		void DrawLineNoDepth(
			const Vector3& P1, const Vector3& P2,
			float Width,
			const Color3F& C1 = Color3F(1, 1, 1), const Color3F& C2 = Color3F(1, 1, 1)
		);
		void DrawLineAABB(
			const Vector3& Min, const Vector3& Max,
			const Vector3& OffsetAll = Vector3(0, 0, 0),
			float Width = 1.0f,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawLineOBB(
			const Entity& entity,
			const Vector3& OffsetAll,
			float Width = 1.0f,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawLineSquare(
			const Vector3& position,
			const Vector3& up,
			const Vector3& right,
			float Width = 1.0f,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawLineScreenSpace(
			const Vector2& P1, const Vector2& P2,
			float Width,
			const Color3F& C1 = Color3F(1, 1, 1), const Color3F& C2 = Color3F(1, 1, 1)
		);
		void DrawLineSquareScreenSpace(
			const Vector2& P, const Vector2& Size,
			float LineWidth,
			const Color3F& Color = Color3F(1, 1, 1)
		);

		// Triangle Drawing
		void DrawCube(
			const Vector3& position,
			const Vector3& scale,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawCube(
			const Matrix4x4& model,
			const Color3F& C = Color3F(1, 1, 1)
		);

		// Triangle Wireframe Drawing
		void DrawWireframeSphere(
			const Vector3& position,
			const Vector3& scale,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawWireframeSphere(
			const Matrix4x4& model,
			const Color3F& C = Color3F(1, 1, 1)
		);

		
		// Rendering
		void RenderWorldLines();
		void RenderWorldLinesNoDepth();
		void RenderScreenLines();
		void End();

	} SingletonInstance(Debug);
}