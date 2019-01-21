// Copyright (c) 2018 Emmanuel Lajeunesse
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
	static class DebugLines : public Singleton<class DebugLines>
	{
		bool m_isInit = false;
		std::vector<float> vertices;
		UINT vertexIndex = 0;
		UINT MaxSizeFloat = 600; // 12 floats = 2 lines // 600 floats = 100 lines
		void Resize(UINT Size)
		{
			MaxSizeFloat = Size;
			vertices.resize(MaxSizeFloat);

			Lines->SetVertices(&vertices[0], (GLuint)vertices.size(), BufferBind_Mode::DYNAMIC);
		}
	private:
		VBO* Lines;
		
	public:
		void Setup()
		{
			delete Lines;
			Lines = new VBO();

			vertices.resize(MaxSizeFloat);

			Lines->SetVertices(&vertices[0], (GLuint)vertices.size(), BufferBind_Mode::DYNAMIC);
			Lines->AddStrideHint(BufferType::VERTEX, 3);
			Lines->AddStrideHint(BufferType::COLOR, 3);

			m_isInit = true;
		}
		void AddLine(
			const Vector3& P1, const Vector3& P2,
			const Color3F& C1, const Color3F& C2
		);
		void DrawLines();

	} SingletonInstance(DebugLines);
}