// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Chunk.h"

#include "../engine/utilities/singleton.h"

namespace Vxl
{
	class Vector3;
	class Vector3i;

	static class TerrainManager : public Singleton<class TerrainManager>
	{
	private:
		bool m_init = false;
		std::unordered_map<UINT, std::unordered_map<UINT, std::unordered_map<UINT, Chunk*>>> m_chunks;
		
		// Utility
		Chunk* NewChunk(Vector3i chunkPosition);
	public:
		// Utility
		Vector3i GetChunkPosition(Vector3 worldPosition);
		Chunk* GetChunk(Vector3i chunkPosition);

		// Behaviour
		void Setup();
		void Draw();

	} SingletonInstance(TerrainManager);

}
