// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Asset.h"
#include "../utilities/Util.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <string>
#include <vector>

namespace Vxl
{
	class Mesh;

	class Model
	{
		friend class Loader;
		friend class Mesh;
	private:
		// Load ASSIMP
		static std::vector<Model*> LoadFromAssimp(
			const std::string& filePath,
			bool mergeMeshes,
			bool normalize,
			float normalizeScale = 1.0f
		);

		// Data
		std::vector<Vector3> positions;
		std::vector<Vector2> uvs;
		std::vector<Vector3> normals;
		std::vector<Vector3> tangents;
		std::vector<Vector3> bitangents;
		//std::vector<Vector4> colors;
		unsigned int vertexCount = 0;

		std::vector<unsigned int> indices;
		unsigned int indexCount = 0;

	public:
		// Load
		static std::vector<Mesh*> Load(
			const std::string& name,
			const std::string& filePath,
			bool mergeMeshes,
			bool normalize,
			float normalizeScale = 1.0f
		);

		// Merge
		void Merge(const Model& other)
		{
			Util::MergeVectors(positions, other.positions);
			Util::MergeVectors(uvs, other.uvs);
			Util::MergeVectors(normals, other.normals);
			Util::MergeVectors(tangents, other.tangents);
			Util::MergeVectors(bitangents, other.bitangents);
			//Util::MergeVectors(colors, other.colors);
			Util::MergeVectors(indices, other.indices);

			vertexCount += other.vertexCount;
			indexCount += other.indexCount;
		}
	};
}