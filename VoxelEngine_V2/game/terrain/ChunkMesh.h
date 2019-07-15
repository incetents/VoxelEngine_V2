// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Block.h"
#include <Windows.h>

#include "../engine/math/Vector.h"

namespace Vxl
{
	class Mesh;
	class Block;

	class ChunkMesh
	{
	private:
		std::vector<Vector3> m_positions;
		std::vector<Vector2> m_uvs;
		std::vector<Vector3> m_normals;
		std::vector<UINT> m_indices;
		UINT m_lastIndex = 0;

		Mesh* m_mesh;

		void AddNormals_Quad(Block::Direction _direction);
		void AddIndices_Quad();

		void AddQuadTop(Vector3i position, Vector4 uvs);
		void AddQuadBot(Vector3i position, Vector4 uvs);
		void AddQuadLeft(Vector3i position, Vector4 uvs);
		void AddQuadRight(Vector3i position, Vector4 uvs);
		void AddQuadFront(Vector3i position, Vector4 uvs);
		void AddQuadBack(Vector3i position, Vector4 uvs);

	public:
		void CreateMesh(Vector3i worldPosition, Block** blocks);
		void Draw();
	};

}
