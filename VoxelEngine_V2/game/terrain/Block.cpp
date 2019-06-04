// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Block.h"

#include "../engine/textures/Texture.h"
#include "../engine/utilities/logger.h"

namespace Vxl
{
	void Block::SetID(Block::ID _id)
	{
		m_base = BlockDictionary.GetBlockBase(_id);
	}
	void Block::SetSubID(UINT subID)
	{
		m_subID = subID;
	}
	const Block::ID Block::GetID(void) const
	{
		return m_base->m_id;
	}
	const Block::Shape Block::GetShape(void) const
	{
		return m_base->m_shape;
	}
	const Vector4 Block::GetUV(Block::Direction _direction) const
	{
		return m_base->m_uvs[(int)_direction];
	}

	void BlockAtlas::Set(Texture* atlas, UINT blockPixelLength)
	{
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

		if (m_uvs != nullptr)
			delete[] m_uvs;

		m_uvs = new Vector4*[m_columns];
		for (UINT i = 0; i < m_columns; i++)
			m_uvs[i] = new Vector4[m_rows];

		// Fill values
		for (UINT x = 0; x < m_columns; x++)
		{
			for (UINT y = 0; y < m_rows; y++)
			{
				float u1 = (float)(x + 0) / (float)m_columns;
				float u2 = (float)(x + 1) / (float)m_columns;
				float v1 = (float)(y + 0) / (float)m_rows;
				float v2 = (float)(y + 1) / (float)m_rows;
				// Flip Y
				float temp = v1;
				v1 = 1.0f - v2;
				v2 = 1.0f - temp;
				//
				m_uvs[x][y] = Vector4(u1, u2, v1, v2);
			}
		}
	}
	Texture* BlockAtlas::GetTexture(void) const
	{
		return m_AtlasTexture;
	}
	void BlockAtlas::BindAtlas()
	{
		assert(m_AtlasTexture);
		m_AtlasTexture->Bind();
	}
	Vector4 BlockAtlas::GetUvs(UINT Column, UINT Row)
	{
		return m_uvs[Column][Row];
	}

	void BlockDictionary::Setup()
	{
		if (m_init)
		{
			for (auto it = m_blocks.begin(); it != m_blocks.end(); it++)
			{
				delete it->second;
			}
			m_blocks.clear();
		}

		// Initialize list of all blocks
		{
			BlockBase* B = new BlockBase(
				Block::ID::AIR,
				Block::Shape::EMPTY,
				BlockBase::UVMap::ALL
			);
			m_blocks[Block::ID::AIR] = B;
		}
		{
			BlockBase* B = new BlockBase(
				Block::ID::STONE,
				Block::Shape::CUBE,
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
		{
			BlockBase* B = new BlockBase(
				Block::ID::GRASS,
				Block::Shape::CUBE,
				BlockBase::UVMap::TOP_AROUND_BOT,
				BlockAtlas.GetUvs(0, 0),
				BlockAtlas.GetUvs(2, 0),
				BlockAtlas.GetUvs(3, 0),
				BlockAtlas.GetUvs(3, 0),
				BlockAtlas.GetUvs(3, 0),
				BlockAtlas.GetUvs(3, 0)
			);
			m_blocks[Block::ID::GRASS] = B;
		}

		m_init = true;
	}
	BlockBase* BlockDictionary::GetBlockBase(Block::ID _id)
	{
		assert(m_init);
		return m_blocks[_id];
	}
}