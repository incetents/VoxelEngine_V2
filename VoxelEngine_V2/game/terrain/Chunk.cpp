// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Chunk.h"

namespace Vxl
{
	const UINT Chunk::X_LENGTH = 8;
	const UINT Chunk::Y_LENGTH = 8;
	const UINT Chunk::Z_LENGTH = 8;

	Chunk::Chunk()
	{
		m_blocks = new Block[X_LENGTH * Y_LENGTH * Z_LENGTH];
	}
	Chunk::~Chunk()
	{
		delete[] m_blocks;
	}
}