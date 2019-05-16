// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "../math/Vector3.h"
#include "../math/Matrix4x4.h"
#include "../math/Color.h"
#include "VBO.h"
#include "glUtil.h"
#include <vector>
#include <assert.h>

namespace Vxl
{
	class Texture;

	static class Debug : public Singleton<class Debug>
	{
		friend class RenderManager;
	private:
		// Debug Lines
		VBO*				m_lines;
		std::vector<float>	m_lines_vertices;  // 16 floats = 2 lines // 800 floats = 100 lines
		UINT				m_lines_vertexIndex = 0;
		bool				m_lines_resizeDirty = false;
		static const UINT	m_lines_vertexIncrementAmount;
		
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
			m_lines_vertices.clear();
			m_lines_vertices.resize(m_lines_vertexIncrementAmount);

			delete m_lines;
			m_lines = new VBO();
			m_lines->SetVertices(m_lines_vertices, BufferBind_Mode::DYNAMIC);
			m_lines->AddStrideHint(BufferType::VERTEX, 3); // loc 0
			m_lines->AddStrideHint(BufferType::COLOR, 4); // loc 3
			m_lines->AddStrideHint(BufferType::UV, 1); // loc 1 (WIDTH)

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

		void UpdateStart();

		void RenderLines();

		// Debug Texture
		Texture* GetNullTexture(void) const
		{
			return m_null_texture;
		}

	} SingletonInstance(Debug);
}