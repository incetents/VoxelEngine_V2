// Copyright(c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "TerrainManager.h"

#include "../engine/math/Vector3.h"

namespace Vxl
{
	Vector3i TerrainManager::GetChunkPosition(Vector3 worldPosition)
	{
		return Vector3i(
			(worldPosition.x >= 0) ? ((int)worldPosition.x / Chunk::X_LENGTH) : (((int)worldPosition.x - Chunk::X_LENGTH) / Chunk::X_LENGTH),
			(worldPosition.y >= 0) ? ((int)worldPosition.y / Chunk::Y_LENGTH) : (((int)worldPosition.y - Chunk::Y_LENGTH) / Chunk::Y_LENGTH),
			(worldPosition.z >= 0) ? ((int)worldPosition.z / Chunk::Z_LENGTH) : (((int)worldPosition.z - Chunk::Z_LENGTH) / Chunk::Z_LENGTH)
		);
	}
	Chunk* TerrainManager::NewChunk(Vector3i chunkPosition)
	{
		Chunk* _chunk = m_chunks[chunkPosition.x][chunkPosition.y][chunkPosition.z];

		if (_chunk)
			delete m_chunks[chunkPosition.x][chunkPosition.y][chunkPosition.z];

		_chunk = new Chunk(
			Vector3i(
				chunkPosition.x * Chunk::X_LENGTH,
				chunkPosition.y * Chunk::Y_LENGTH,
				chunkPosition.z * Chunk::Z_LENGTH
			)
		);

		m_chunks[chunkPosition.x][chunkPosition.y][chunkPosition.z] = _chunk;

		return _chunk;
	}
	Chunk* TerrainManager::GetChunk(Vector3i chunkPosition)
	{
		if (m_chunks.find(chunkPosition.x) == m_chunks.end())
		{
			std::unordered_map<UINT, std::unordered_map<UINT, Chunk*>> map;
			m_chunks[chunkPosition.x] = map;
		}
		if (m_chunks[chunkPosition.x].find(chunkPosition.y) == m_chunks[chunkPosition.x].end())
		{
			std::unordered_map<UINT, Chunk*> map;
			m_chunks[chunkPosition.x][chunkPosition.y] = map;
		}
		if (m_chunks[chunkPosition.x][chunkPosition.y].find(chunkPosition.z) == m_chunks[chunkPosition.x][chunkPosition.y].end())
		{
			m_chunks[chunkPosition.x][chunkPosition.y][chunkPosition.z] = nullptr;
		}

		return m_chunks[chunkPosition.x][chunkPosition.y][chunkPosition.z];
	}
	


	void TerrainManager::Setup()
	{
		if (m_init)
			return;

		Chunk* C1 = NewChunk(Vector3i(0, 0, 0));
		C1->Rebuild();

		Chunk* C2 = NewChunk(Vector3i(1, 0, 0));
		C2->Rebuild();

		m_init = true;
	}

	void TerrainManager::Draw()
	{
		if (m_chunks[0][0][0])
			m_chunks[0][0][0]->Draw();

		if (m_chunks[1][0][0])
			m_chunks[1][0][0]->Draw();
	}
}