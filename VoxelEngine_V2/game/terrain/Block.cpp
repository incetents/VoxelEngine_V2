// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Block.h"

#include "../engine/opengl/Texture.h"
#include "../engine/utilities/logger.h"

namespace Vxl
{
	void BlockAtlas::Setup(Texture* atlas, UINT blockPixelLength)
	{
		if (m_setup)
		{
			assert(false);
			return;
		}

		if (atlas == nullptr)
		{
			Logger.error("BlockAtlas using nullptr texture");
			assert(false);
			return;
		}

		m_AtlasTexture = atlas;
		m_blockPixelLength = blockPixelLength;

		m_columns = m_AtlasTexture->GetWidth() / blockPixelLength;
		m_rows = m_AtlasTexture->GetHeight() / blockPixelLength;

		m_uvs = new Vector4*[m_columns];
		for (UINT i = 0; i < m_columns; i++)
			m_uvs[i] = new Vector4[m_rows];

		// Fill values
		for (UINT x = 0; x < m_columns; x++)
		{
			for (UINT y = 0; y < m_rows; y++)
			{
				m_uvs[x][y] = Vector4
				(
					(float)(x + 0) / (float)m_columns,
					(float)(x + 1) / (float)m_columns,
					(float)(y + 0) / (float)m_rows,
					(float)(y + 1) / (float)m_rows
				);
			}
		}

		m_setup = true;
	}
	Vector4 BlockAtlas::GetUvs(UINT Column, UINT Row)
	{
		assert(m_setup);
		// Existing uv set
		return m_uvs[Column][Row];
	}

	void BlockDictionary::Setup()
	{
		// Initialize list of all blocks
		{
			BlockBase* B = new BlockBase(
				Block::ID::AIR,
				BlockBase::Shape::CUBE,
				BlockBase::UVMap::ALL
			);
			m_blocks[Block::ID::AIR] = B;
		}
		{
			BlockBase* B = new BlockBase(
				Block::ID::STONE,
				BlockBase::Shape::CUBE,
				BlockBase::UVMap::ALL,
				BlockAtlas.GetUvs(1, 0),
				BlockAtlas.GetUvs(1, 0),
				BlockAtlas.GetUvs(1, 0),
				BlockAtlas.GetUvs(1, 0),
				BlockAtlas.GetUvs(1, 0),
				BlockAtlas.GetUvs(1, 0)
			);
			m_blocks[Block::ID::STONE] = B;
		}
	}
}