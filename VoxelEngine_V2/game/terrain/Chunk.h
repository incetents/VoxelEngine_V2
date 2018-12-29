// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "ChunkMesh.h"
#include "Block.h"

namespace Vxl
{
	class Chunk
	{
		friend class ChunkMesh;
	public:
		const static UINT X_LENGTH;
		const static UINT Y_LENGTH;
		const static UINT Z_LENGTH;
		const static UINT VOLUME;

	private:
		ChunkMesh m_chunkMesh;
		Block** m_blocks;
		Vector3i m_worldPosition;

		static bool CheckIndexOutOfRange(int x, int y, int z);
		static UINT GetIndex(UINT x, UINT y, UINT z);
		static UINT GetIndex(Vector3i pos);
		void SetBlock(Vector3i pos, Block::ID id, UINT subID = 0);
	public:
		Chunk(Vector3i worldPosition);
		~Chunk();

		void Rebuild();
		void Draw();
	};
}

