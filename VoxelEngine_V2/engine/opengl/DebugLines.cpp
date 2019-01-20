// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "DebugLines.h"

namespace Vxl
{
	void DebugLines::AddLine(
		const Vector3& P1, const Vector3& P2,
		const Color3F& C1, const Color3F& C2
	)
	{
		if (vertexIndex >= MaxSizeFloat)
		{
			Resize(MaxSizeFloat + 600);
		}

		vertices[vertexIndex + 0] = P1.x;
		vertices[vertexIndex + 1] = P1.y;
		vertices[vertexIndex + 2] = P1.z;
		vertices[vertexIndex + 3] = C1.r;
		vertices[vertexIndex + 4] = C1.g;
		vertices[vertexIndex + 5] = C1.b;

		vertices[vertexIndex + 6] = P2.x;
		vertices[vertexIndex + 7] = P2.y;
		vertices[vertexIndex + 8] = P2.z;
		vertices[vertexIndex + 9] = C2.r;
		vertices[vertexIndex + 10] = C2.g;
		vertices[vertexIndex + 11] = C2.b;

		vertexIndex += 12;
	}

	void DebugLines::DrawLines()
	{
		Lines.Bind();

		// Update Vertices
		glUtil::bindVBOSubData(0, MaxSizeFloat * sizeof(float), &vertices[0]);

		Lines.Draw(Draw_Type::LINES);
		vertexIndex = 0;
	}
}