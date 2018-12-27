// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "../engine/math/Vector4.h"
#include "../engine/utilities/singleton.h"

#include <Windows.h>
#include <unordered_map>
#include <assert.h>

namespace Vxl
{
	class Texture;

	class Block
	{
	private:

	public:
		enum ID
		{
			AIR = 0,
			STONE,
			GRASS
		};
		enum Direction
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT,
			FRONT,
			BACK,
			TOTAL
		};
	};

	class BlockBase
	{
		friend class BlockDictionary;
	private:
		enum Shape
		{
			CUBE = 0
		};
		enum UVMap
		{
			ALL,
			TOP_AROUND_BOT
		};
	public:
		BlockBase(
			Block::ID id,
			Shape shape,
			UVMap map,
			Vector4 uvUp = Vector4(),
			Vector4 uvDown = Vector4(),
			Vector4 uvLeft = Vector4(),
			Vector4 uvRight = Vector4(),
			Vector4 uvFront = Vector4(),
			Vector4 uvBack = Vector4()
		) : m_id(id), m_shape(shape), m_UVmap(map)
		{
			m_uvs[0] = uvUp;
			m_uvs[1] = uvDown;
			m_uvs[2] = uvLeft;
			m_uvs[3] = uvRight;
			m_uvs[4] = uvFront;
			m_uvs[5] = uvBack;
		}

		const Block::ID m_id;
		const Shape		m_shape;
		const UVMap		m_UVmap;
		Vector4			m_uvs[6];
	};

	static class BlockAtlas : public Singleton<class BlockDictionary>
	{
	private:
		bool		m_setup = false;
		Texture*	m_AtlasTexture;
		UINT		m_blockPixelLength;
		UINT		m_columns;
		UINT		m_rows;
		Vector4**	m_uvs;
	public:
		void Setup(Texture* atlas, UINT blockPixelLength);
		Vector4 GetUvs(UINT Column, UINT Row);

	} SingletonInstance(BlockAtlas);

	static class BlockDictionary : public Singleton<class BlockDictionary>
	{
	private:
		std::unordered_map<Block::ID, BlockBase*> m_blocks;
	public:
		void Setup();

	} SingletonInstance(BlockDictionary);
}