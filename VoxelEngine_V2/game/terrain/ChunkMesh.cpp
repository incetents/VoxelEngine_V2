// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "ChunkMesh.h"


#include "Chunk.h"
#include "../engine/opengl/Mesh.h"

namespace Vxl
{
	void ChunkMesh::AddNormals_Quad(Block::Direction _direction)
	{
		Vector3 Normal;
		switch (_direction)
		{
		case Block::Direction::UP:
			Normal = Vector3(0, +1, 0);
			break;
		case Block::Direction::DOWN:
			Normal = Vector3(0, -1, 0);
			break;
		case Block::Direction::LEFT:
			Normal = Vector3(-1, 0, 0);
			break;
		case Block::Direction::RIGHT:
			Normal = Vector3(+1, 0, 0);
			break;
		case Block::Direction::FRONT:
			Normal = Vector3(0, 0, +1);
			break;
		case Block::Direction::BACK:
			Normal = Vector3(0, 0, -1);
			break;
		}
		for(UINT i = 0; i < 4; i++)
			m_normals.push_back(Normal);
	}
	void ChunkMesh::AddIndices_Quad()
	{
		m_indices.push_back(m_lastIndex + 0);
		m_indices.push_back(m_lastIndex + 1);
		m_indices.push_back(m_lastIndex + 2);
		m_indices.push_back(m_lastIndex + 0);
		m_indices.push_back(m_lastIndex + 2);
		m_indices.push_back(m_lastIndex + 3);
		m_lastIndex = m_indices[m_indices.size() - 1] + 1;
	}

	void ChunkMesh::AddQuadTop(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 1.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::UP);
		AddIndices_Quad();
	}
	void ChunkMesh::AddQuadBot(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 1.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::DOWN);
		AddIndices_Quad();
	}
	void ChunkMesh::AddQuadLeft(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 0.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::LEFT);
		AddIndices_Quad();
	}
	void ChunkMesh::AddQuadRight(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 1.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::RIGHT);
		AddIndices_Quad();
	}
	void ChunkMesh::AddQuadFront(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 1.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 1.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::FRONT);
		AddIndices_Quad();
	}
	void ChunkMesh::AddQuadBack(Vector3i position, Vector4 uvs)
	{
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 0.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 0.0f, position.y + 1.0f, position.z + 0.0f));
		m_positions.push_back(Vector3(position.x + 1.0f, position.y + 1.0f, position.z + 0.0f));

		m_uvs.push_back(Vector2(uvs.x, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.z));
		m_uvs.push_back(Vector2(uvs.y, uvs.w));
		m_uvs.push_back(Vector2(uvs.x, uvs.w));

		AddNormals_Quad(Block::Direction::BACK);
		AddIndices_Quad();
	}


	void ChunkMesh::CreateMesh(Vector3i worldPosition, Block** blocks)
	{
		m_positions.clear();
		m_uvs.clear();
		m_normals.clear();
		m_indices.clear();
		m_lastIndex = 0;

		if (m_mesh)
			delete m_mesh;

		for (UINT x = 0; x < Chunk::X_LENGTH; x++)
		{
			for (UINT y = 0; y < Chunk::Y_LENGTH; y++)
			{
				for (UINT z = 0; z < Chunk::Z_LENGTH; z++)
				{
					// Index
					Block* _block = blocks[Chunk::GetIndex(x, y, z)];
					assert(_block);
					Block::Shape _shape = _block->GetShape();
					int _x = (int)x;
					int _y = (int)y;
					int _z = (int)z;
					Vector3i BlockPosition = Vector3i(x, y, z) + worldPosition;

					if (_shape != Block::Shape::EMPTY)
					{
						switch (_shape)
						{
						case Block::Shape::CUBE:
						{
							// Top
							if (!Chunk::CheckIndexOutOfRange(_x, _y + 1, _z))
							{
								if(blocks[Chunk::GetIndex(x, y + 1, z)]->GetShape() == Block::Shape::EMPTY)
									AddQuadTop(BlockPosition, _block->GetUV(Block::Direction::UP));
							}
							else
								AddQuadTop(BlockPosition, _block->GetUV(Block::Direction::UP));
							// Bottom
							if (!Chunk::CheckIndexOutOfRange(_x, _y - 1, _z))
							{
								if (blocks[Chunk::GetIndex(x, y - 1, z)]->GetShape() == Block::Shape::EMPTY)
									AddQuadBot(BlockPosition, _block->GetUV(Block::Direction::DOWN));
							}
							else
								AddQuadBot(BlockPosition, _block->GetUV(Block::Direction::DOWN));
							// Left
							if (!Chunk::CheckIndexOutOfRange(_x - 1, _y, _z))
							{
								if (blocks[Chunk::GetIndex(x - 1, y, z)]->GetShape() == Block::Shape::EMPTY)
									AddQuadLeft(BlockPosition, _block->GetUV(Block::Direction::LEFT));
							}
							else
								AddQuadLeft(BlockPosition, _block->GetUV(Block::Direction::LEFT));
							// Right
							if (!Chunk::CheckIndexOutOfRange(_x + 1, _y, _z))
							{
								if (blocks[Chunk::GetIndex(x + 1, y, z)]->GetShape() == Block::Shape::EMPTY)
									AddQuadRight(BlockPosition, _block->GetUV(Block::Direction::RIGHT));
							}
							else
								AddQuadRight(BlockPosition, _block->GetUV(Block::Direction::RIGHT));
							// Front
							if (!Chunk::CheckIndexOutOfRange(_x, _y, _z + 1))
							{
								if (blocks[Chunk::GetIndex(x, y, z + 1)]->GetShape() == Block::Shape::EMPTY)
									AddQuadFront(BlockPosition, _block->GetUV(Block::Direction::FRONT));
							}
							else
								AddQuadFront(BlockPosition, _block->GetUV(Block::Direction::FRONT));
							// Back
							if (!Chunk::CheckIndexOutOfRange(_x, _y, _z - 1))
							{
								if (blocks[Chunk::GetIndex(x, y, z - 1)]->GetShape() == Block::Shape::EMPTY)
									AddQuadBack(BlockPosition, _block->GetUV(Block::Direction::BACK));
							}
							else
								AddQuadBack(BlockPosition, _block->GetUV(Block::Direction::BACK));

							break;
						}
						default:
							assert(false);
						}

					}
				}
			}
		}

		m_mesh = new Mesh();
		m_mesh->m_positions.set(m_positions);
		m_mesh->m_uvs.set(m_uvs);
		m_mesh->m_normals.set(m_normals);
		m_mesh->m_indices.set(m_indices);

		m_mesh->Bind();
	}

	void ChunkMesh::Draw()
	{
		m_mesh->Draw();
	}
}