// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "../math/Vector.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"

#include "../modules/Entity.h"
#include "../rendering/LineMesh.h"

#include "VBO.h"

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
				m_mesh = new LineMesh(type == PositionType::VEC3);
			}

			void AddLine_Vec3(
				const Vector3& P1, const Vector3& P2,
				float Width,
				const Color4F& C1, const Color4F& C2
			) {
				VXL_ASSERT(m_type == PositionType::VEC3, "Invalid LineSet Type for this function");

				// Vertices
				std::vector<float>& vertices = m_mesh->m_buffer.getVerticesEditing();

				// Resize if reached vector size limit
				if (m_mesh->m_vertexIndex >= vertices.size())
				{
					vertices.resize(vertices.size() + VertexIncrementAmount);
					m_mesh->m_resizeDirty = true;
				}

				vertices[m_mesh->m_vertexIndex + 0] = P1.x;
				vertices[m_mesh->m_vertexIndex + 1] = P1.y;
				vertices[m_mesh->m_vertexIndex + 2] = P1.z;
				vertices[m_mesh->m_vertexIndex + 3] = C1.r;
				vertices[m_mesh->m_vertexIndex + 4] = C1.g;
				vertices[m_mesh->m_vertexIndex + 5] = C1.b;
				vertices[m_mesh->m_vertexIndex + 6] = C1.a;

				vertices[m_mesh->m_vertexIndex + 7] = Width;

				vertices[m_mesh->m_vertexIndex + 8] = P2.x;
				vertices[m_mesh->m_vertexIndex + 9] = P2.y;
				vertices[m_mesh->m_vertexIndex + 10] = P2.z;
				vertices[m_mesh->m_vertexIndex + 11] = C2.r;
				vertices[m_mesh->m_vertexIndex + 12] = C2.g;
				vertices[m_mesh->m_vertexIndex + 13] = C2.b;
				vertices[m_mesh->m_vertexIndex + 14] = C2.a;

				vertices[m_mesh->m_vertexIndex + 15] = Width;

				// Offset counter
				m_mesh->m_vertexIndex += 16;

			}
			void AddLine_Vec2(
				const Vector2& P1, const Vector2& P2,
				float Width,
				const Color4F& C1, const Color4F& C2
			) {
				VXL_ASSERT(m_type == PositionType::VEC2, "Invalid LineSet Type for this function");

				// Vertices
				std::vector<float>& vertices = m_mesh->m_buffer.getVerticesEditing();

				// Resize if reached vector size limit
				if (m_mesh->m_vertexIndex >= vertices.size())
				{
					vertices.resize(vertices.size() + VertexIncrementAmount);
					m_mesh->m_resizeDirty = true;
				}

				vertices[m_mesh->m_vertexIndex + 0] = P1.x;
				vertices[m_mesh->m_vertexIndex + 1] = P1.y;
				vertices[m_mesh->m_vertexIndex + 2] = C1.r;
				vertices[m_mesh->m_vertexIndex + 3] = C1.g;
				vertices[m_mesh->m_vertexIndex + 4] = C1.b;
				vertices[m_mesh->m_vertexIndex + 5] = C1.a;

				vertices[m_mesh->m_vertexIndex + 6] = Width;

				vertices[m_mesh->m_vertexIndex + 7] = P2.x;
				vertices[m_mesh->m_vertexIndex + 8] = P2.y;
				vertices[m_mesh->m_vertexIndex + 9] = C2.r;
				vertices[m_mesh->m_vertexIndex + 10] = C2.g;
				vertices[m_mesh->m_vertexIndex + 11] = C2.b;
				vertices[m_mesh->m_vertexIndex + 12] = C2.a;

				vertices[m_mesh->m_vertexIndex + 13] = Width;

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