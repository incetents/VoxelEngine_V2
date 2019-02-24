// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Database.h"

#include "Vector2.h"
#include "Vector3.h"

#include <string>
#include <vector>

namespace Vxl
{
	class Model
	{
	private:
		// Database
		static Database<Model> m_database;

		// Load ASSIMP
		static std::vector<Model*> LoadFromAssimp(const std::string& filePath);
	public:
		// Data
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;
		std::vector<Vector3> tangents;
		std::vector<Vector3> bitangents;
		unsigned int vertexCount = 0;

		std::vector<unsigned int> indices;
		unsigned int indexCount = 0;

		// Database Creation
		static bool Create(const std::string& name, const std::string& filePath);
		// Database Getter
		static Model* Get(const std::string& name)
		{
			return m_database.Get(name);
		}
	};
}