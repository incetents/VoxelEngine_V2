// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../utilities/stringUtil.h"
#include "../utilities/Logger.h"
#include "../rendering/Mesh.h"

namespace Vxl
{
	Vector4 InterpretAssimpVec4(aiColor4D vec)
	{
		return Vector4(vec.r, vec.g, vec.b, vec.a);
	}
	Vector3 InterpretAssimpVec3(aiVector3D vec)
	{
		return Vector3(vec.x, vec.y, vec.z);
	}
	Vector2 InterpretAssimpVec2(aiVector3D vec)
	{
		return Vector2(vec.x, vec.y);
	}

	std::vector<Model*> Model::LoadFromAssimp(
		const std::string& filePath,
		bool mergeMeshes,
		bool normalize,
		float normalizeScale
	) {
		unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality;

		const aiScene* scene = aiImportFile(filePath.c_str(), flags);

		if (!scene)
		{
			// Error
			auto Name = stringUtil::nameFromFilepath(filePath);
			Logger.error("Unable to load Model: " + Name);
			return std::vector<Model*>();
		}

		// Start
		std::vector<Model*> _models;
		_models.reserve(scene->mNumMeshes);

		float LongestVertexSqr = 0.0f;

		// Iterate each mesh
		for (std::uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			aiMesh* mesh = scene->mMeshes[meshIndex];
			if (!mesh->HasPositions())
				continue;

			// Start
			Model* _model = new Model();
			_model->vertexCount = mesh->mNumVertices;
			_model->indexCount = mesh->mNumFaces * 3u;

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
			// colors
			//	if (mesh->HasVertexColors(0))
			//		_model->colors.reserve(mesh->mNumVertices);

			// add all data
			for (std::uint32_t vertIndex = 0u; vertIndex < mesh->mNumVertices; vertIndex++)
			{
				_model->positions.push_back(InterpretAssimpVec3(mesh->mVertices[vertIndex]));

				// Longest position
				if (normalize)
					LongestVertexSqr = _model->positions.back().LengthSqr();

				if (mesh->HasNormals())
					_model->normals.push_back(InterpretAssimpVec3(mesh->mNormals[vertIndex]));
				if (mesh->HasTextureCoords(0))
					_model->uvs.push_back(InterpretAssimpVec2(mesh->mTextureCoords[0][vertIndex]));
				if (mesh->HasTangentsAndBitangents())
				{
					_model->tangents.push_back(InterpretAssimpVec3(mesh->mTangents[vertIndex]));
					_model->bitangents.push_back(InterpretAssimpVec3(mesh->mBitangents[vertIndex]));
				}
				//if (mesh->HasVertexColors(0))
				//{
				//	_model->colors.push_back(InterpretAssimpVec4(mesh->mColors[0][vertIndex]));
				//}
			}

			// index
			_model->indices.reserve(mesh->mNumFaces * 3u);
			for (std::uint32_t faceIndex = 0u; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				VXL_ASSERT(mesh->mFaces[faceIndex].mNumIndices == 3u, "Model Loader, indices are not in a multiples of 3");
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[0u]);
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[1u]);
				_model->indices.push_back(mesh->mFaces[faceIndex].mIndices[2u]);
			}

			// End
			_models.push_back(_model);
		}

		// Merge possibility
		if (mergeMeshes)
		{
			int MeshCount = (int)_models.size();
			for (int i = 1; i < MeshCount; i++)
			{
				_models[0]->Merge(*_models[i]);
				delete _models[i];
			}
			_models.resize(1);
		}

		// normalize size
		if (normalize)
		{
			float LongestVertex = sqrt(LongestVertexSqr);
			unsigned int MeshCount = (int)_models.size();
			for (unsigned int M = 0; M < MeshCount; M++)
			{
				unsigned int PositionCount = (unsigned int)_models[M]->positions.size();
				for (unsigned int vertIndex = 0u; vertIndex < PositionCount; vertIndex++)
				{
					_models[M]->positions[vertIndex] = _models[M]->positions[vertIndex] * normalizeScale / LongestVertex;
				}
			}
		}
		

		// End
		aiReleaseImport(scene);
		return _models;
	}

	std::vector<MeshIndex> Model::LoadMeshes(
		const std::string& name,
		const std::string& filePath,
		bool normalize,
		float normalizeScale
	) {
		std::vector<MeshIndex> _meshes;
		std::vector<Model*> Models = Model::LoadFromAssimp(filePath, false, normalize, normalizeScale);
		UINT _modelCount = (UINT)Models.size();

		if (_modelCount == 0)
		{
			Logger.error("Unable to Load Model: " + name);
			return _meshes;
		}

		for (UINT i = 0; i < _modelCount; i++)
		{
			std::string _name = name;
			if (_modelCount > 1)
				_name += " (" + std::to_string(i) + ')';

			// Index
			MeshIndex NewMeshIndex = SceneAssets.createMesh();
			_meshes.push_back(NewMeshIndex);

			// Mesh Data
			Mesh* _mesh = SceneAssets.getMesh(NewMeshIndex);
			_mesh->Set(*Models[i]);
			_mesh->setGLName(_name);
		}

		return _meshes;
	}
	MeshIndex Model::LoadMesh(
		const std::string& name,
		const std::string& filePath,
		bool normalize,
		float normalizeScale
	)
	{
		std::vector<Model*> Models = Model::LoadFromAssimp(filePath, false, normalize, normalizeScale);
		if (Models.size() == 0)
			return -1;

		MeshIndex NewMeshIndex = SceneAssets.createMesh();
		Mesh* _mesh = SceneAssets.getMesh(NewMeshIndex);
		_mesh->Set(*Models[0]);
		_mesh->setGLName(name);

		return NewMeshIndex;
	}
}