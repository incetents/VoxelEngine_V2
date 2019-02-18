// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../utilities/stringUtil.h"
#include "../utilities/Logger.h"

namespace Vxl
{
	Database<Model> Model::m_database;

	Vector3 InterpretAssimpVec3(aiVector3D vec)
	{
		return Vector3(vec.x, vec.y, vec.z);
	}
	Vector2 InterpretAssimpVec2(aiVector3D vec)
	{
		return Vector2(vec.x, vec.y);
	}

	std::vector<Model*> Model::LoadFromAssimp(const std::string& filePath)
	{
		unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality;

		const aiScene* scene = aiImportFile(filePath.c_str(), flags);

		if (!scene)
		{
			// Error
			auto Name = stringUtil::nameFromFilepath(filePath);
			return std::vector<Model*>();
		}

		// Start
		std::vector<Model*> _models;
		_models.reserve(scene->mNumMeshes);

		// Iterate each mesh
		for (std::uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			aiMesh* mesh = scene->mMeshes[meshIndex];
			if (!mesh->HasPositions())
				continue;

			// Start
			Model* _model = new Model();
			// position space
			_model->positions.reserve(mesh->mNumVertices);
			if (mesh->HasNormals())
				_model->normals.reserve(mesh->mNumVertices);
			// uv space
			if (mesh->HasTextureCoords(0))
				_model->uvs.reserve(mesh->mNumVertices);
			// tangent space
			if (mesh->HasTangentsAndBitangents())
			{
				_model->tangents.reserve(mesh->mNumVertices);
				_model->bitangents.reserve(mesh->mNumVertices);
			}

			for (std::uint32_t vertIndex = 0u; vertIndex < mesh->mNumVertices; vertIndex++)
			{
				_model->positions.push_back(InterpretAssimpVec3(mesh->mVertices[vertIndex]));
				if (mesh->HasNormals())
					_model->normals.push_back(InterpretAssimpVec3(mesh->mNormals[vertIndex]));
				if (mesh->HasTextureCoords(0))
					_model->uvs.push_back(InterpretAssimpVec2(mesh->mTextureCoords[0][vertIndex]));
				if (mesh->HasTangentsAndBitangents())
				{
					_model->tangents.push_back(InterpretAssimpVec3(mesh->mTangents[vertIndex]));
					_model->bitangents.push_back(InterpretAssimpVec3(mesh->mBitangents[vertIndex]));
				}
			}

			// index
			_model->indices.reserve(mesh->mNumFaces * 3u);
			for (std::uint32_t faceIndex = 0u; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				assert(mesh->mFaces[faceIndex].mNumIndices == 3u);
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[0u]);
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[1u]);
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[2u]);
			}

			// End
			_models.push_back(_model);
		}

		// End
		aiReleaseImport(scene);
		return _models;
	}

	bool Model::Create(const std::string& name, const std::string& filePath)
	{
		// Name Duplication
		if (m_database.Check(name))
		{
			Logger.error("Duplicate Model: " + name);
			return false;
		}

		std::vector<Model*> Models = LoadFromAssimp(filePath);
		UINT ModelCount = (UINT)Models.size();

		if (ModelCount == 0)
		{
			Logger.error("Unable to Load Model: " + name);
			return false;
		}
		else if (ModelCount == 1)
		{
			Logger.log("Loaded Model: " + name);
			m_database.Set(name, Models[0]);
			return true;
		}
		else
		{
			for (UINT i = 0; i < ModelCount; i++)
			{
				// New Names for extra meshes
				std::string NameFix = name;
				if(i > 0)
					NameFix = name + "(" + std::to_string(i) + ")";

				Logger.log("Loaded Model: " + NameFix);
				m_database.Set(NameFix, Models[i]);
			}
			return true;
		}
	}
}