// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

#include "../math/Vector3.h"
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
	private:
		// Debug Lines
		VBO*				m_lines;
		std::vector<float>	m_lines_vertices;  // 12 floats = 2 lines // 600 floats = 100 lines
		UINT				m_lines_vertexIndex = 0;
		bool				m_lines_resizeDirty = false;
		// Debug Textures
		Texture*			m_null_texture;

	public:
		// Debug Lines
		void Setup()
		{
			m_lines_vertices.clear();
			m_lines_vertices.resize(600);

			delete m_lines;
			m_lines = new VBO();
			m_lines->SetVertices(m_lines_vertices, BufferBind_Mode::DYNAMIC);
			m_lines->AddStrideHint(BufferType::VERTEX, 3);
			m_lines->AddStrideHint(BufferType::COLOR, 3);
		}
		void DrawLine(
			const Vector3& P1, const Vector3& P2,
			const Color3F& C1 = Color3F(1, 1, 1), const Color3F& C2 = Color3F(1, 1, 1)
		);
		void DrawAABB(
			const Vector3& Min, const Vector3& Max, const Vector3& OffsetAll = Vector3(0, 0, 0),
			const Color3F& C = Color3F(1, 1, 1)
		);
		void UpdateStart();
		void RenderLines();
		// Debug Textures
		void CreateDebugTextures();
		void BindNullTexture();

	} SingletonInstance(Debug);
}