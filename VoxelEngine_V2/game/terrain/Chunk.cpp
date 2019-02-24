// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Chunk.h"

namespace Vxl
{
	const UINT Chunk::X_LENGTH = 6;
	const UINT Chunk::Y_LENGTH = 6;
	const UINT Chunk::Z_LENGTH = 6;
	const UINT Chunk::VOLUME = X_LENGTH * Y_LENGTH * Z_LENGTH;

	Chunk::Chunk(Vector3i worldPosition)
		: m_worldPosition(worldPosition)
	{
		m_blocks = new Block*[VOLUME];
		for (UINT i = 0; i < VOLUME; i++)
		{
			m_blocks[i] = new Block(BlockDictionary.GetBlockBase(Block::ID::AIR));
		}

		SetBlock(Vector3i(0, 0, 0), Block::ID::STONE);
		SetBlock(Vector3i(1, 0, 0), Block::ID::STONE);
		SetBlock(Vector3i(3, 0, 0), Block::ID::STONE);
		SetBlock(Vector3i(5, 0, 0), Block::ID::STONE);
		SetBlock(Vector3i(5, 1, 0), Block::ID::STONE);

		SetBlock(Vector3i(2, 0, 2), Block::ID::GRASS);
		SetBlock(Vector3i(2, 1, 2), Block::ID::STONE);
		SetBlock(Vector3i(2, 2, 2), Block::ID::GRASS);

	}
	Chunk::~Chunk()
	{
		delete[] m_blocks;
	}

	bool Chunk::CheckIndexOutOfRange(int x, int y, int z)
	{
		return (
			x < 0 || x >= Chunk::X_LENGTH ||
			y < 0 || y >= Chunk::Y_LENGTH ||
			z < 0 || z >= Chunk::Z_LENGTH
			);
	}
	UINT Chunk::GetIndex(UINT x, UINT y, UINT z)
	{
		return x + y * Chunk::X_LENGTH + z * Chunk::X_LENGTH * Chunk::Y_LENGTH;
	}
	UINT Chunk::GetIndex(Vector3i pos)
	{
		return pos.x + pos.y * Chunk::X_LENGTH + pos.z * Chunk::X_LENGTH * Chunk::Y_LENGTH;
	}
	void Chunk::SetBlock(Vector3i pos, Block::ID id, UINT subID)
	{
		UINT index = GetIndex(pos);
		m_blocks[index]->SetID(id);
		m_blocks[index]->SetSubID(subID);
	}

	void Chunk::Rebuild()
	{
		m_chunkMesh.CreateMesh(m_worldPosition, m_blocks);
	}
	void Chunk::Draw()
	{
		m_chunkMesh.Draw();
	}
}