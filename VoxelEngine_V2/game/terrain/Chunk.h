// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Block.h"

namespace Vxl
{
	class Chunk
	{
	public:
		const static UINT X_LENGTH;
		const static UINT Y_LENGTH;
		const static UINT Z_LENGTH;

	private:
		Block* m_blocks;
	public:
		Chunk();
		~Chunk();
	};
}

