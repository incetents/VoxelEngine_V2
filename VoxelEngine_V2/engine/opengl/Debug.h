// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "VBO.h"
#include "glUtil.h"

#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

#include "../modules/Entity.h"
#include "../opengl/LineMesh.h"

#include <vector>
#include <assert.h>

namespace Vxl
{
	class Texture;

	static class Debug : public Singleton<class Debug>
	{
		friend class RenderManager;

		static const UINT	VertexIncrementAmount;

		

		class LineSet
		{
		public:
			enum class PositionType
			{
				VEC2,
				VEC3
			};
			PositionType		m_type; // Whether position in VBO stride has 2 or 3 values

			LineMesh*			m_mesh;

			void Setup(UINT size, PositionType type)
			{
				m_type = type;

				delete m_mesh;
				m_mesh = new LineMesh();

				if (type == PositionType::VEC3)
					m_mesh->AddStrideHint(BufferType::VERTEX, 3); // loc 0
				else
					m_mesh->AddStrideHint(BufferType::VERTEX, 2); // loc 0

				m_mesh->AddStrideHint(BufferType::COLOR, 4);  // loc 3
				m_mesh->AddStrideHint(BufferType::LINEWIDTH, 1); // loc 1

				m_mesh->SetVertices();
				m_mesh->Bind();
			}
			void AddLine_Vec3(
				const Vector3& P1, const Vector3& P2,
				float Width,
				const Color4F& C1, const Color4F& C2
			) {
				assert(m_type == PositionType::VEC3);

				if (m_mesh->m_vertexIndex >= m_mesh->m_vertices.size())
				{
					m_mesh->m_vertices.resize(m_mesh->m_vertices.size() + VertexIncrementAmount);
					m_mesh->m_resizeDirty = true;
				}

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 0] = P1.x;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 1] = P1.y;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 2] = P1.z;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 3] = C1.r;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 4] = C1.g;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 5] = C1.b;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 6] = C1.a;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 7] = Width;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 8] = P2.x;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 9] = P2.y;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 10] = P2.z;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 11] = C2.r;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 12] = C2.g;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 13] = C2.b;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 14] = C2.a;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 15] = Width;

				// Offset counter
				m_mesh->m_vertexIndex += 16;

			}
			void AddLine_Vec2(
				const Vector2& P1, const Vector2& P2,
				float Width,
				const Color4F& C1, const Color4F& C2
			) {
				assert(m_type == PositionType::VEC2);

				if (m_mesh->m_vertexIndex >= m_mesh->m_vertices.size())
				{
					m_mesh->m_vertices.resize(m_mesh->m_vertices.size() + VertexIncrementAmount);
					m_mesh->m_resizeDirty = true;
				}

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 0] = P1.x;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 1] = P1.y;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 2] = C1.r;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 3] = C1.g;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 4] = C1.b;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 5] = C1.a;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 6] = Width;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 7] = P2.x;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 8] = P2.y;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 9] = C2.r;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 10] = C2.g;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 11] = C2.b;
				m_mesh->m_vertices[m_mesh->m_vertexIndex + 12] = C2.a;

				m_mesh->m_vertices[m_mesh->m_vertexIndex + 13] = Width;

				// Offset counter
				m_mesh->m_vertexIndex += 14;
			}

		};

	private:
		// Debug Lines in world space
		LineSet m_worldLines;
		// Debug Lines in screen space
		LineSet m_screenLines;

		
		// Debug Wireframe Spheres
		struct WireframeSphere
		{
			Color4F color;
			Matrix4x4 model;
			void calcModel(Vector3 center, Vector3 scale)
			{
				model = Matrix4x4::GetScale(scale);
				model.OverrideCenter(center);
			}
		};
		std::vector<WireframeSphere> m_wireframeSpheres;
		
		// Debug Textures
		void CreateDebugTextures();
		Texture*			m_null_texture;

	public:
		// Debug Lines
		void Setup()
		{
			m_worldLines.Setup(VertexIncrementAmount, LineSet::PositionType::VEC3);
			m_screenLines.Setup(VertexIncrementAmount, LineSet::PositionType::VEC2);

			CreateDebugTextures();
		}
		void DrawLine(
			const Vector3& P1, const Vector3& P2,
			float Width,
			const Color4F& C1 = Color4F(1, 1, 1, 1), const Color4F& C2 = Color4F(1, 1, 1, 1)
		);
		void DrawAABB(
			const Vector3& Min, const Vector3& Max,
			const Vector3& OffsetAll = Vector3(0, 0, 0),
			float Width = 1.0f,
			const Color4F& C = Color4F(1, 1, 1, 1)
		);
		void DrawOBB(
			const Entity& entity,
			const Vector3& OffsetAll,
			float Width = 1.0f,
			const Color4F& C = Color4F(1, 1, 1, 1)
		);
		void DrawSquare(
			const Vector3& position,
			const Vector3& up,
			const Vector3& right,
			float Width = 1.0f,
			const Color4F& C = Color4F(1, 1, 1, 1)
		);
		void DrawWireframeSphere(
			const Vector3& position,
			const Vector3& scale,
			const Color4F& C = Color4F(1, 1, 1, 1)
		);
		void DrawWireframeSphere(
			const Matrix4x4& model,
			const Color4F& C = Color4F(1, 1, 1, 1)
		);
		
		void DrawScreenSpaceLine(
			const Vector2& P1, const Vector2& P2,
			float Width,
			const Color4F& C1 = Color4F(1, 1, 1, 1), const Color4F& C2 = Color4F(1, 1, 1, 1)
		);
		void DrawScreenSpaceSquare(
			const Vector2& P, const Vector2& Size,
			float LineWidth,
			const Color4F& Color = Color4F(1, 1, 1, 1)
		);

		void RenderWorldLines();
		void RenderScreenLines();
		void End();

		// Debug Texture
		Texture* GetNullTexture(void) const
		{
			return m_null_texture;
		}

	} SingletonInstance(Debug);
}