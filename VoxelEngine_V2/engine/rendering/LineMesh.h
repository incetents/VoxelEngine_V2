// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "MeshBuffer.h"
#include "VBO.h"
#include "Graphics.h"

#include "../rendering/MeshBuffer.h"
#include "../utilities/Types.h"

#include <vector>

namespace Vxl
{
	class LineMesh //: public Asset<LineMesh>
	{
	private:
		VAO m_VAO;

		DrawType m_type;
		uint32_t m_drawCount = 0;	// Vertices Drawn

	public:
		MeshBufferMem<float> m_buffer;

		UINT				m_vertexIndex = 0;
		bool				m_resizeDirty = false;

		LineMesh(bool isVec3);

		void bind(DrawType type = DrawType::LINES);
		void Draw();
	};
}

