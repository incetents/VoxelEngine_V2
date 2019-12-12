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

	static class Debug : public Singleton<class Debug>
	{
		friend class RenderManager;

	public:
		// Debug Object
		struct Object
		{
			Color3F		color;
			Matrix4x4	model;
		};

		// Storage
		std::vector<Object> m_spheres;
		std::vector<Object> m_spheresLines;
		std::vector<Object> m_cubes;
		std::vector<Object> m_cubesLines;

	private:
		// Debug Lines in world space
		LineMesh3D* m_worldLines;
		LineMesh3D* m_worldLinesNoDepth;
		// Debug Lines in screen space
		LineMesh2D* m_screenLines;

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
		void DrawLineCube(
			const Vector3& position,
			float width,
			const Vector3& scale = Vector3(1, 1, 1),
			const Vector3& eulerRotation = Vector3(0, 0, 0),
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawLineSphere(
			const Vector3& position,
			const Vector3& scale,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawLineAABB(
			const Vector3& Min, const Vector3& Max,
			const Color3F& C = Color3F(1, 1, 1)
		);

		void DrawLineOBB(
			const Entity& entity,
			const Vector3& OffsetAll,
			float Width = 1.0f,
			const Color3F& C = Color3F(1, 1, 1)
		);
		//	void DrawLineSquare(
		//		const Vector3& position,
		//		const Vector3& up,
		//		const Vector3& right,
		//		float Width = 1.0f,
		//		const Color3F& C = Color3F(1, 1, 1)
		//	);
		

		// Outlines
		

		//	void DrawAABBOutline(
		//		const Vector3& position,
		//		const Vector3& min,
		//		const Vector3& max,
		//		const Color3F& C = Color3F(1, 1, 1)
		//	);

		// 3D Drawing
		void DrawCube(
			const Vector3& position,
			const Vector3& scale = Vector3(1, 1, 1),
			const Vector3& eulerRotation = Vector3(0, 0, 0),
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawCube(
			const Matrix4x4& model,
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawSphere(
			const Vector3& position,
			const Vector3& scale = Vector3(1, 1, 1),
			const Vector3& eulerRotation = Vector3(0, 0, 0),
			const Color3F& C = Color3F(1, 1, 1)
		);
		void DrawSphere(
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