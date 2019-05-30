// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Geometry.h"

#include "Mesh.h"

#include "../math/MathCore.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace Vxl
{
	// Vertex Maker
	void Geometry::CreateCylinder(
		std::vector<Vector3>& pos, std::vector<Vector2>& uvs,
		Axis axis, UINT slices, float height, float radius_top, float radius_bot,
		const Vector3& offset
	){
		if (axis != Axis::Y)
			std::swap(radius_top, radius_bot);

		pos.clear();
		uvs.clear();
		pos.reserve(12 * slices);
		uvs.reserve(12 * slices);

		float uv_height_total = height + radius_bot + radius_top;
		float uv_height_bot = (radius_bot / uv_height_total);
		float uv_height_top = (height + radius_bot) / uv_height_total;

		float half_height = height / 2;

		for (unsigned int i = 0; i < slices; i++)
		{
			float t1 = TWO_PI * ((float)i / (float)slices);
			float t2 = TWO_PI * ((float)(i + 1) / (float)slices);

			float uvx1 = ((float)i / (float)slices);
			float uvx2 = ((float)(i + 1) / (float)slices);

			Vector2 c1 = Vector2(sinf(t1), cosf(t1));
			Vector2 c2 = Vector2(sinf(t2), cosf(t2));

			Vector3 p1 = Vector3(c1.x * radius_top, +half_height, c1.y * radius_top);
			Vector3 p2 = Vector3(c2.x * radius_top, +half_height, c2.y * radius_top);
			Vector3 p3 = Vector3(c1.x * radius_bot, -half_height, c1.y * radius_bot);
			Vector3 p4 = Vector3(c2.x * radius_bot, -half_height, c2.y * radius_bot);

			// Circle Top
			pos.push_back(Vector3(0, half_height, 0));
			pos.push_back(p1);
			pos.push_back(p2);

			uvs.push_back(Vector2(0.5f, 0.5f));
			uvs.push_back(c1 * 0.5f + 0.5f);
			uvs.push_back(c2 * 0.5f + 0.5f);

			// Circle Bottom
			pos.push_back(Vector3(0, -half_height, 0));
			pos.push_back(p4);
			pos.push_back(p3);

			uvs.push_back(Vector2(0.5f, 0.5f));
			uvs.push_back(c2 * 0.5f + 0.5f);
			uvs.push_back(c1 * 0.5f + 0.5f);

			// Side
			pos.push_back(p1);
			pos.push_back(p3);
			pos.push_back(p2);

			uvs.push_back(Vector2(uvx1, uv_height_top));
			uvs.push_back(Vector2(uvx1, uv_height_bot));
			uvs.push_back(Vector2(uvx2, uv_height_top));

			pos.push_back(p2);
			pos.push_back(p3);
			pos.push_back(p4);

			uvs.push_back(Vector2(uvx2, uv_height_top));
			uvs.push_back(Vector2(uvx1, uv_height_bot));
			uvs.push_back(Vector2(uvx2, uv_height_bot));
		}

		// Fix Axis
		if (axis == Axis::X)
		{
			for (auto& p : pos)
			{
				p.SwapXY();
				p.x = -p.x;
				p += offset;
			}
		}
		else if (axis == Axis::Z)
		{
			for (auto& p : pos)
			{
				p.SwapYZ();
				p.z = -p.z;
				p += offset;
			}
		}
		else
			for (auto& p : pos)
				p += offset;
	}
	void Geometry::CreateCone(
		std::vector<Vector3>& pos, std::vector<Vector2>& uvs,
		Axis axis, UINT slices, float height, float radius,
		const Vector3& offset
	){
		pos.clear();
		uvs.clear();
		pos.reserve(12 * slices);
		uvs.reserve(12 * slices);

		float half_height = height / 2;

		for (unsigned int i = 0; i < slices; i++)
		{
			float t1 = TWO_PI * ((float)i / (float)slices);
			float t2 = TWO_PI * ((float)(i + 1) / (float)slices);

			float uvx1 = ((float)i / (float)slices);
			float uvx2 = ((float)(i + 1) / (float)slices);

			Vector2 c1 = Vector2(sinf(t1), cosf(t1));
			Vector2 c2 = Vector2(sinf(t2), cosf(t2));


			Vector3 p3 = Vector3(c1.x * radius, -half_height, c1.y * radius);
			Vector3 p4 = Vector3(c2.x * radius, -half_height, c2.y * radius);

			if (axis != Axis::Y)
			{
				Vector3 tmp = p3;
				p3 = p4;
				p4 = tmp;
			}
			// Circle Bottom
			pos.push_back(Vector3(0, -half_height, 0));
			pos.push_back(p4);
			pos.push_back(p3);

			uvs.push_back(Vector2(0.5f, 0.5f));
			uvs.push_back(c2 * 0.5f + 0.5f);
			uvs.push_back(c1 * 0.5f + 0.5f);

			// Side
			pos.push_back(Vector3(0, half_height, 0));
			pos.push_back(p3);
			pos.push_back(p4);

			uvs.push_back(Vector2(uvx2, 1.0f));
			uvs.push_back(Vector2(uvx1, 0.5f));
			uvs.push_back(Vector2(uvx2, 0.5f));
		}

		// Fix Axis
		if (axis == Axis::X)
		{
			for (auto& p : pos)
			{
				p.SwapXY();
				p += offset;
			}
		}
		else if (axis == Axis::Z)
		{
			for (auto& p : pos)
			{
				p.SwapYZ();
				p += offset;
			}
		}
		else
			for (auto& p : pos)
				p += offset;
	}

	// Generators
	Mesh* Geometry::GenerateIcosahdron(const std::string& MeshName, unsigned int subdivisions, float scale)
	{
		float t = (1.0f + sqrt(5.0f)) / 2.0f;

		std::vector<Vector3> m_points;
		m_points.reserve(12);

		m_points.push_back(Vector3(-1, +t, 0).NormalizeSelf());
		m_points.push_back(Vector3(+1, +t, 0).NormalizeSelf());
		m_points.push_back(Vector3(-1, -t, 0).NormalizeSelf());
		m_points.push_back(Vector3(+1, -t, 0).NormalizeSelf());

		m_points.push_back(Vector3(0, -1, +t).NormalizeSelf());
		m_points.push_back(Vector3(0, +1, +t).NormalizeSelf());
		m_points.push_back(Vector3(0, -1, -t).NormalizeSelf());
		m_points.push_back(Vector3(0, +1, -t).NormalizeSelf());

		m_points.push_back(Vector3(+t, 0, -1).NormalizeSelf());
		m_points.push_back(Vector3(+t, 0, +1).NormalizeSelf());
		m_points.push_back(Vector3(-t, 0, -1).NormalizeSelf());
		m_points.push_back(Vector3(-t, 0, +1).NormalizeSelf());

		std::vector<Vector3i> m_faces;
		m_faces.reserve(20);

		m_faces.push_back(Vector3i(0, 11, 5));
		m_faces.push_back(Vector3i(0, 5, 1));
		m_faces.push_back(Vector3i(0, 1, 7));
		m_faces.push_back(Vector3i(0, 7, 10));
		m_faces.push_back(Vector3i(0, 10, 11));

		m_faces.push_back(Vector3i(1, 5, 9));
		m_faces.push_back(Vector3i(5, 11, 4));
		m_faces.push_back(Vector3i(11, 10, 2));
		m_faces.push_back(Vector3i(10, 7, 6));
		m_faces.push_back(Vector3i(7, 1, 8));

		m_faces.push_back(Vector3i(3, 9, 4));
		m_faces.push_back(Vector3i(3, 4, 2));
		m_faces.push_back(Vector3i(3, 2, 6));
		m_faces.push_back(Vector3i(3, 6, 8));
		m_faces.push_back(Vector3i(3, 8, 9));

		m_faces.push_back(Vector3i(4, 9, 5));
		m_faces.push_back(Vector3i(2, 4, 11));
		m_faces.push_back(Vector3i(6, 2, 10));
		m_faces.push_back(Vector3i(8, 6, 7));
		m_faces.push_back(Vector3i(9, 8, 1));

		// Recursion
		GLuint faceCount = (GLuint)m_faces.size();
		GLuint offset = 0;
		for (GLuint i = 0; i < subdivisions; i++)
		{
			std::vector<Vector3> NewPoints;
			std::vector<Vector3i> NewFaces;
			Vec3 Tri[3];
			Vec3 Mid[3];
			for (GLuint i = 0; i < faceCount; i++)
			{
				Vector3i face = m_faces[i];
				Tri[0] = m_points[face.x];
				Tri[1] = m_points[face.y];
				Tri[2] = m_points[face.z];
				Mid[0] = Vec3::Lerp(Tri[0], Tri[1], 0.5f);
				Mid[1] = Vec3::Lerp(Tri[1], Tri[2], 0.5f);
				Mid[2] = Vec3::Lerp(Tri[2], Tri[0], 0.5f);

				NewPoints.push_back(Tri[0].Normalize());
				NewPoints.push_back(Tri[1].Normalize());
				NewPoints.push_back(Tri[2].Normalize());
				NewPoints.push_back(Mid[0].Normalize());
				NewPoints.push_back(Mid[1].Normalize());
				NewPoints.push_back(Mid[2].Normalize());

				offset = 6 * i;
				NewFaces.push_back(Vector3i(offset + 0, offset + 3, offset + 5));
				NewFaces.push_back(Vector3i(offset + 3, offset + 1, offset + 4));
				NewFaces.push_back(Vector3i(offset + 5, offset + 4, offset + 2));
				NewFaces.push_back(Vector3i(offset + 3, offset + 4, offset + 5));
			}
			m_faces = NewFaces;
			m_points = NewPoints;
			faceCount = (GLuint)m_faces.size();
		}

		// Find North/South Poles of isocahedron
		float NorthPole = 0;
		float SouthPole = 0;

		int PositionCount = (int)m_points.size();
		for (int i = 0; i < PositionCount; i++)
		{
			Vector3 v = m_points[i];

			if (v.y > NorthPole)
				NorthPole = v.y;
			else if (v.y < SouthPole)
				SouthPole = v.y;
		}

		std::vector<Vector3> vertices;
		vertices.reserve(faceCount * 3);
		std::vector<Vector2> uvs;
		uvs.reserve(faceCount * 3);
		Vec3 Tri[3];
		Vec3 Normals[3];
		Vec2 UVs[3];
		// Solve for UV
		for (GLuint i = 0; i < faceCount; i++)
		{
			Tri[0] = m_points[m_faces[i].x];
			Tri[1] = m_points[m_faces[i].y];
			Tri[2] = m_points[m_faces[i].z];

			Normals[0] = Tri[0].Normalize();
			Normals[1] = Tri[1].Normalize();
			Normals[2] = Tri[2].Normalize();

			for (UINT j = 0; j < 3; j++)
			{
				UVs[j] = Vector2(
					(atan2(Normals[j].z, Normals[j].x) + PI) / PI / 2,
					(acos(Normals[j].y / 1.0f) + PI) / PI - 1
				);
			}

			// Check for uv flip
			Vector3 TexA(UVs[0].x, UVs[0].y, 0);
			Vector3 TexB(UVs[1].x, UVs[1].y, 0);
			Vector3 TexC(UVs[2].x, UVs[2].y, 0);
			Vector3 TexNormal = Vector3::Cross((TexC - TexB), (TexA - TexB));
			bool WrappedUV = (TexNormal.z < 0);
			// Offset uvs to match up
			if (WrappedUV)
			{
				for (int j = 0; j < 3; j++)
				{
					if (UVs[j].x < 0.25)
						UVs[j].x += 1;
				}

			}
			// Fix swirl on polar coordinates
			for (int j = 0; j < 3; j++)
			{
				if (fabs(Tri[j].y - NorthPole) <= FUZZ ||
					fabs(Tri[j].y - SouthPole) <= FUZZ
					) {
					if (subdivisions > 0)
						UVs[j].x = (UVs[(j + 1) % 3].x + UVs[(j + 2) % 3].x) / 2.0f;

					// !!!!!!
					// It is unknown how to fix UVS properly for Isocahedron without subdivisions
				}

			}

			vertices.push_back(Tri[0] * scale);
			vertices.push_back(Tri[1] * scale);
			vertices.push_back(Tri[2] * scale);

			uvs.push_back(UVs[0]);
			uvs.push_back(UVs[1]);
			uvs.push_back(UVs[2]);
		}

		Mesh* NewMesh = Mesh::Create(MeshName);
		//
		NewMesh->m_positions.set(vertices);
		NewMesh->m_uvs.set(uvs);
		NewMesh->GenerateNormals(true);
		NewMesh->GenerateTangents();
		//
		NewMesh->Bind();
		return NewMesh;
	}
	Mesh* Geometry::GenerateSphereUV(const std::string& MeshName, unsigned int xSlice, unsigned int ySlice)
	{
		std::vector<Vector3> positions;
		positions.reserve(xSlice * ySlice * 6);
		std::vector<Vector2> uvs;
		uvs.reserve(xSlice * ySlice * 6);
		std::vector<Vector3> normals;
		normals.reserve(xSlice * ySlice * 6);
		bool invert = false;
		float x = (float)xSlice;
		float y = (float)ySlice;

		for (int i = 0; i < x; ++i)
		{
			for (int j = 0; j < y; ++j)
			{
				float s0 = sin(((j + 0) / y) * PI);
				float s1 = sin(((j + 1) / y) * PI);

				float c0 = cosf((2.0f * (j + 0) / y - 1.0f)* HALF_PI + HALF_PI);
				float c1 = cosf((2.0f * (j + 1) / y - 1.0f)* HALF_PI + HALF_PI);

				vec3 blPos = vec3(s0 * sinf((i + 0) * TWO_PI / x), c0, s0 * cosf((i + 0) * TWO_PI / x)) / 2.0f;
				vec3 brPos = vec3(s0 * sinf((i + 1) * TWO_PI / x), c0, s0 * cosf((i + 1) * TWO_PI / x)) / 2.0f;
				vec3 tlPos = vec3(s1 * sinf((i + 0) * TWO_PI / x), c1, s1 * cosf((i + 0) * TWO_PI / x)) / 2.0f;
				vec3 trPos = vec3(s1 * sinf((i + 1) * TWO_PI / x), c1, s1 * cosf((i + 1) * TWO_PI / x)) / 2.0f;

				if (!invert)
				{
					positions.push_back(blPos);
					positions.push_back(tlPos);
					positions.push_back(brPos);
					positions.push_back(tlPos);
					positions.push_back(trPos);
					positions.push_back(brPos);
				}
				else
				{
					positions.push_back(blPos);
					positions.push_back(brPos);
					positions.push_back(tlPos);
					positions.push_back(tlPos);
					positions.push_back(brPos);
					positions.push_back(trPos);
				}

				vec2 bl = vec2((i) / x, 1.f - (j) / y);
				vec2 br = vec2((i + 1) / x, 1.f - (j) / y);
				vec2 tl = vec2((i) / x, 1.f - (j + 1) / y);
				vec2 tr = vec2((i + 1) / x, 1.f - (j + 1) / y);

				if (!invert)
				{
					uvs.push_back(bl);
					uvs.push_back(tl);
					uvs.push_back(br);
					uvs.push_back(tl);
					uvs.push_back(tr);
					uvs.push_back(br);
				}
				else
				{
					uvs.push_back(bl);
					uvs.push_back(br);
					uvs.push_back(tl);
					uvs.push_back(tl);
					uvs.push_back(br);
					uvs.push_back(tr);
				}
			}
		}

		for (const auto& p : positions)
			normals.push_back(p.Normalize());

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		_mesh->m_positions.set(positions);
		_mesh->m_uvs.set(uvs);
		_mesh->m_normals.set(normals);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}
	Mesh* Geometry::GenerateCylinder(const std::string& MeshName, Axis axis, UINT slices, float height, float radius_top, float radius_bot)
	{
		std::vector<Vector3> positions;
		std::vector<Vector2> uvs;

		CreateCylinder(positions, uvs, axis, slices, height, radius_top, radius_bot, Vector3::ZERO);

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		_mesh->m_positions.set(positions);
		_mesh->m_uvs.set(uvs);
		_mesh->GenerateNormals(true);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}
	Mesh* Geometry::GenerateCone(const std::string& MeshName, Axis axis, UINT slices, float height, float radius)
	{
		std::vector<Vector3> positions;
		std::vector<Vector2> uvs;

		CreateCone(positions, uvs, axis, slices, height, radius, Vector3::ZERO);

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		_mesh->m_positions.set(positions);
		_mesh->m_uvs.set(uvs);
		_mesh->GenerateNormals(true);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}
	Mesh* Geometry::GenerateArrow(const std::string& MeshName, Axis axis, float tailLength, const Vector3& offset)
	{
		std::vector<Vector3> positions;
		std::vector<Vector3> positions2;
		std::vector<Vector2> uvs;
		std::vector<Vector2> uvs2;
		// RESERVE
		Vector3 p1Offset;
		Vector3 p2Offset;
		float stemHeight = 0.66f;
		float tipHeight = 0.33f;
		p1Offset[(int)axis] = stemHeight * 0.5f;
		p2Offset[(int)axis] = stemHeight + tipHeight * 0.5f;;

		CreateCylinder(positions, uvs, axis, 16, stemHeight, 0.08f, 0.08f, p1Offset);
		CreateCone(positions2, uvs2, axis, 16, tipHeight, 0.2f, p2Offset);

		positions.insert(positions.end(), positions2.begin(), positions2.end());
		uvs.insert(uvs.end(), uvs2.begin(), uvs2.end());

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		_mesh->m_positions.set(positions);
		_mesh->m_uvs.set(uvs);
		_mesh->GenerateNormals(true);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}
	Mesh* Geometry::GenerateCube(const std::string& MeshName, float unitSize)
	{
		Vector3 pos[] = {
			// Front Face
			Vector3(-0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
			Vector3(+0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			Vector3(-0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			//
			Vector3(-0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			Vector3(-0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			//
			Vector3(-0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			Vector3(-0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			//
			Vector3(-0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
			Vector3(-0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
			//
			Vector3(-0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(-0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			Vector3(-0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			Vector3(-0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
			//
			Vector3(+0.5f * unitSize, -0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, -0.5f * unitSize),
			Vector3(+0.5f * unitSize, +0.5f * unitSize, +0.5f * unitSize),
			Vector3(+0.5f * unitSize, -0.5f * unitSize, +0.5f * unitSize),
		};
		Vector2 uvs[] = {
			//
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
			//
			Vector2(1, 0),
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			//
			Vector2(0, 1),
			Vector2(1, 1),
			Vector2(1, 0),
			Vector2(0, 0),
			//
			Vector2(1, 0),
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			//
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			Vector2(1, 0),
			//
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
			Vector2(0, 0),
		};
		Vector3 normals[] = {
			//
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			//
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			//
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			//
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			//
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			//
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
		};
		GLuint indices[] = {
			0, 1, 2, 0, 2, 3,
			4, 6, 5, 6, 4, 7,
			9, 8, 10, 10, 8, 11,
			12, 13, 14, 12, 14, 15,
			17, 16, 18, 18, 16, 19,
			21, 22, 20, 22, 23, 20
		};

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		_mesh->m_positions.set(pos, 24);
		_mesh->m_uvs.set(uvs, 24);
		_mesh->m_normals.set(normals, 24);
		_mesh->m_indices.set(indices, 36);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}
	Mesh* Geometry::GenerateQuad(const std::string& MeshName, Axis axis)
	{
		
		Vector2 uvs[] = {
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1)
		};

		GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

		Mesh* _mesh = Mesh::Create(MeshName);
		//
		if (axis == Axis::X)
		{
			Vector3 pos[] = {
				Vector3(+0.0f, -0.5f, +0.5f),
				Vector3(+0.0f, -0.5f, -0.5f),
				Vector3(+0.0f, +0.5f, -0.5f),
				Vector3(+0.0f, +0.5f, +0.5f)
			};
			_mesh->m_positions.set(pos, 4);

			Vector3 normals[] = {
				Vector3(+1, 0, 0),
				Vector3(+1, 0, 0),
				Vector3(+1, 0, 0),
				Vector3(+1, 0, 0)
			};
			_mesh->m_normals.set(normals, 4);
		}
		else if (axis == Axis::Y)
		{
			Vector3 pos[] = {
				Vector3(-0.5f, 0, +0.5f),
				Vector3(+0.5f, 0, +0.5f),
				Vector3(+0.5f, 0, -0.5f),
				Vector3(-0.5f, 0, -0.5f)
			};
			_mesh->m_positions.set(pos, 4);

			Vector3 normals[] = {
				Vector3(0, +1, 0),
				Vector3(0, +1, 0),
				Vector3(0, +1, 0),
				Vector3(0, +1, 0)
			};
			_mesh->m_normals.set(normals, 4);
		}
		else if (axis == Axis::Z)
		{
			Vector3 pos[] = {
				Vector3(-0.5f, -0.5f, +0.0f),
				Vector3(+0.5f, -0.5f, +0.0f),
				Vector3(+0.5f, +0.5f, +0.0f),
				Vector3(-0.5f, +0.5f, +0.0f),
			};
			_mesh->m_positions.set(pos, 4);

			Vector3 normals[] = {
				Vector3(0, 0, +1),
				Vector3(0, 0, +1),
				Vector3(0, 0, +1),
				Vector3(0, 0, +1)
			};
			_mesh->m_normals.set(normals, 4);
		}

		_mesh->m_uvs.set(uvs, 4);
		_mesh->m_indices.set(indices, 6);
		_mesh->GenerateTangents();
		//
		_mesh->Bind();
		return _mesh;
	}

	// Creators
	void Geometry::CreateFullQuad()
	{
		Vector3 pos[] = {
			Vector3(-1, -1, +0),
			Vector3(+1, -1, +0),
			Vector3(+1, +1, +0),
			Vector3(-1, +1, +0),
		};
		Vector2 uvs[] = {
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1)
		};
		//Vector3 normals[] = {
		//	Vector3(0, 0, +1),
		//	Vector3(0, 0, +1),
		//	Vector3(0, 0, +1),
		//	Vector3(0, 0, +1)
		//};
		GLuint indices[] = {0, 1, 2, 0, 2, 3};

		m_fullQuad = Mesh::Create("FullQuad");
		//
		m_fullQuad->m_positions.set(pos, 4);
		m_fullQuad->m_uvs.set(uvs, 4);
		//m_fullQuad->m_normals.set(normals, 4);
		m_fullQuad->m_indices.set(indices, 6);
		//
		m_fullQuad->Bind();
	}
	void Geometry::CreateFullTriangle()
	{
		Vector3 pos[] = {
			Vector3(-1, -1, +0),
			Vector3(+3, -1, +0),
			Vector3(-1, +3, +0)
		};
		Vector2 uvs[] = {
			Vector2(0, 0),
			Vector2(2, 0),
			Vector2(0, 2)
		};
		//Vector3 normals[] = {
		//	Vector3(0, 0, +1),
		//	Vector3(0, 0, +1),
		//	Vector3(0, 0, +1)
		//};
		GLuint indices[] = { 0, 1, 2 };

		m_fullTriangle = Mesh::Create("FullTriangle");
		//
		m_fullTriangle->m_positions.set(pos, 3);
		m_fullTriangle->m_uvs.set(uvs, 3);
		//m_fullTriangle->m_normals.set(normals, 3);
		m_fullTriangle->m_indices.set(indices, 6);
		//
		m_fullTriangle->Bind();
	}

	void Geometry::CreateQuad()
	{
		m_quad_x = GenerateQuad("Quad_X", Axis::X);
		m_quad_y = GenerateQuad("Quad_Y", Axis::Y);
		m_quad_z = GenerateQuad("Quad_Z", Axis::Z);
		//	Vector3 pos[] = {
		//		Vector3(-0.5f, -0.5f, +0.0f),
		//		Vector3(+0.5f, -0.5f, +0.0f),
		//		Vector3(+0.5f, +0.5f, +0.0f),
		//		Vector3(-0.5f, +0.5f, +0.0f),
		//	};
		//	Vector2 uvs[] = {
		//		Vector2(0, 0),
		//		Vector2(1, 0),
		//		Vector2(1, 1),
		//		Vector2(0, 1)
		//	};
		//	Vector3 normals[] = {
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1)
		//	};
		//	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
		//	
		//	m_quad = Mesh::Create("Quad");
		//	//
		//	m_quad->m_positions.set(pos, 4);
		//	m_quad->m_uvs.set(uvs, 4);
		//	m_quad->m_normals.set(normals, 4);
		//	m_quad->m_indices.set(indices, 6);
		//	m_quad->GenerateTangents();
		//	//
		//	m_quad->Bind();
	}	

	void Geometry::CreateCube()
	{
		m_cube = GenerateCube("Cube", 1.0f);
		m_cube_small = GenerateCube("Cube_small", 0.25f);
		//	Vector3 pos[] = {
		//		// Front Face
		//		Vector3(-0.5f, -0.5f, +0.5f),
		//		Vector3(+0.5f, -0.5f, +0.5f),
		//		Vector3(+0.5f, +0.5f, +0.5f),
		//		Vector3(-0.5f, +0.5f, +0.5f),
		//		//
		//		Vector3(-0.5f, -0.5f, -0.5f),
		//		Vector3(+0.5f, -0.5f, -0.5f),
		//		Vector3(+0.5f, +0.5f, -0.5f),
		//		Vector3(-0.5f, +0.5f, -0.5f),
		//		//
		//		Vector3(-0.5f, +0.5f, -0.5f),
		//		Vector3(+0.5f, +0.5f, -0.5f),
		//		Vector3(+0.5f, +0.5f, +0.5f),
		//		Vector3(-0.5f, +0.5f, +0.5f),
		//		//
		//		Vector3(-0.5f, -0.5f, -0.5f),
		//		Vector3(+0.5f, -0.5f, -0.5f),
		//		Vector3(+0.5f, -0.5f, +0.5f),
		//		Vector3(-0.5f, -0.5f, +0.5f),
		//		//
		//		Vector3(-0.5f, -0.5f, -0.5f),
		//		Vector3(-0.5f, +0.5f, -0.5f),
		//		Vector3(-0.5f, +0.5f, +0.5f),
		//		Vector3(-0.5f, -0.5f, +0.5f),
		//		//
		//		Vector3(+0.5f, -0.5f, -0.5f),
		//		Vector3(+0.5f, +0.5f, -0.5f),
		//		Vector3(+0.5f, +0.5f, +0.5f),
		//		Vector3(+0.5f, -0.5f, +0.5f),
		//	};
		//	Vector2 uvs[] = {
		//		//
		//		Vector2(0, 0),
		//		Vector2(1, 0),
		//		Vector2(1, 1),
		//		Vector2(0, 1),
		//		//
		//		Vector2(1, 0),
		//		Vector2(0, 0),
		//		Vector2(0, 1),
		//		Vector2(1, 1),
		//		//
		//		Vector2(0, 1),
		//		Vector2(1, 1),
		//		Vector2(1, 0),
		//		Vector2(0, 0),
		//		//
		//		Vector2(1, 0),
		//		Vector2(0, 0),
		//		Vector2(0, 1),
		//		Vector2(1, 1),
		//		//
		//		Vector2(0, 0),
		//		Vector2(0, 1),
		//		Vector2(1, 1),
		//		Vector2(1, 0),
		//		//
		//		Vector2(1, 0),
		//		Vector2(1, 1),
		//		Vector2(0, 1),
		//		Vector2(0, 0),
		//	};
		//	Vector3 normals[] = {
		//		//
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1),
		//		Vector3(0, 0, +1),
		//		//
		//		Vector3(0, 0, -1),
		//		Vector3(0, 0, -1),
		//		Vector3(0, 0, -1),
		//		Vector3(0, 0, -1),
		//		//
		//		Vector3(0, +1, 0),
		//		Vector3(0, +1, 0),
		//		Vector3(0, +1, 0),
		//		Vector3(0, +1, 0),
		//		//
		//		Vector3(0, -1, 0),
		//		Vector3(0, -1, 0),
		//		Vector3(0, -1, 0),
		//		Vector3(0, -1, 0),
		//		//
		//		Vector3(-1, 0, 0),
		//		Vector3(-1, 0, 0),
		//		Vector3(-1, 0, 0),
		//		Vector3(-1, 0, 0),
		//		//
		//		Vector3(+1, 0, 0),
		//		Vector3(+1, 0, 0),
		//		Vector3(+1, 0, 0),
		//		Vector3(+1, 0, 0),
		//	};
		//	GLuint indices[] = {
		//		0, 1, 2, 0, 2, 3,
		//		4, 6, 5, 6, 4, 7,
		//		9, 8, 10, 10, 8, 11,
		//		12, 13, 14, 12, 14, 15,
		//		17, 16, 18, 18, 16, 19,
		//		21, 22, 20, 22, 23, 20
		//	};
		//	
		//	m_cube = Mesh::Create("Cube");
		//	//
		//	m_cube->m_positions.set(pos, 24);
		//	m_cube->m_uvs.set(uvs, 24);
		//	m_cube->m_normals.set(normals, 24);
		//	m_cube->m_indices.set(indices, 36);
		//	m_cube->GenerateTangents();
		//	//
		//	m_cube->Bind();
	}

	void Geometry::CreateInverseCube()
	{
		Vector3 pos[] = {
			// Front Face
			Vector3(-0.5f, -0.5f, +0.5f),
			Vector3(+0.5f, -0.5f, +0.5f),
			Vector3(+0.5f, +0.5f, +0.5f),
			Vector3(-0.5f, +0.5f, +0.5f),
			//
			Vector3(-0.5f, -0.5f, -0.5f),
			Vector3(+0.5f, -0.5f, -0.5f),
			Vector3(+0.5f, +0.5f, -0.5f),
			Vector3(-0.5f, +0.5f, -0.5f),
			//
			Vector3(-0.5f, +0.5f, -0.5f),
			Vector3(+0.5f, +0.5f, -0.5f),
			Vector3(+0.5f, +0.5f, +0.5f),
			Vector3(-0.5f, +0.5f, +0.5f),
			//
			Vector3(-0.5f, -0.5f, -0.5f),
			Vector3(+0.5f, -0.5f, -0.5f),
			Vector3(+0.5f, -0.5f, +0.5f),
			Vector3(-0.5f, -0.5f, +0.5f),
			//
			Vector3(-0.5f, -0.5f, -0.5f),
			Vector3(-0.5f, +0.5f, -0.5f),
			Vector3(-0.5f, +0.5f, +0.5f),
			Vector3(-0.5f, -0.5f, +0.5f),
			//
			Vector3(+0.5f, -0.5f, -0.5f),
			Vector3(+0.5f, +0.5f, -0.5f),
			Vector3(+0.5f, +0.5f, +0.5f),
			Vector3(+0.5f, -0.5f, +0.5f),
		};
		Vector2 uvs[] = {
			//
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
			//
			Vector2(1, 0),
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			//
			Vector2(0, 1),
			Vector2(1, 1),
			Vector2(1, 0),
			Vector2(0, 0),
			//
			Vector2(1, 0),
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			//
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 1),
			Vector2(1, 0),
			//
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1),
			Vector2(0, 0),
		};
		Vector3 normals[] = {
			//
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			Vector3(0, 0, -1),
			//
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			//
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			Vector3(0, -1, 0),
			//
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			Vector3(0, +1, 0),
			//
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
			Vector3(+1, 0, 0),
			//
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
			Vector3(-1, 0, 0),
		};
		GLuint indices[] = {
			1, 0, 2, 2, 0, 3,
			6, 4, 5, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			13, 12, 14, 14, 12, 15,
			16, 17, 18, 16, 18, 19,
			22, 21, 20, 23, 22, 20
		};

		m_inverseCube = Mesh::Create("InverseCube");
		//
		m_inverseCube->m_positions.set(pos, 24);
		m_inverseCube->m_uvs.set(uvs, 24);
		m_inverseCube->m_normals.set(normals, 24);
		m_inverseCube->m_indices.set(indices, 36);
		m_inverseCube->GenerateTangents();
		//
		m_inverseCube->Bind();
	}

	void Geometry::CreateOctahedron()
	{
		Vector3 pos[] = {
			// Edge
			Vector3(-SQRT_HALF, 0, -SQRT_HALF)/2,
			Vector3(+SQRT_HALF, 0, -SQRT_HALF)/2,
			Vector3(+SQRT_HALF, 0, +SQRT_HALF)/2,
			Vector3(-SQRT_HALF, 0, +SQRT_HALF)/2,
			// Top Bottom
			Vector3(0, +0.5f, 0.0f),
			Vector3(0, -0.5f, 0.0f)
		};
		Vector3 normals[] = {
			// Edge
			Vector3(-SQRT_HALF, 0, -SQRT_HALF),
			Vector3(+SQRT_HALF, 0, -SQRT_HALF),
			Vector3(+SQRT_HALF, 0, +SQRT_HALF),
			Vector3(-SQRT_HALF, 0, +SQRT_HALF),
			// Top Bottom
			Vector3(0, +1.0f, 0.0f),
			Vector3(0, -1.0f, 0.0f)
		};
		Vector2 uvs[] = {
			Vector2(0.00f, 0.5f),
			Vector2(0.25f, 0.5f),
			Vector2(0.50f, 0.5f),
			Vector2(1.00f, 0.5f),

			Vector2(0.5f, 1.0f),
			Vector2(0.5f, 0.0f)
		};
		GLuint indices[] = {
			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
			3, 4, 0,
			1, 5, 0,
			2, 5, 1,
			3, 5, 2,
			0, 5, 3
		};

		m_octahedron = Mesh::Create("Octahedron");
		//
		m_octahedron->m_positions.set(pos, 6);
		m_octahedron->m_uvs.set(uvs, 6);
		m_octahedron->m_normals.set(normals, 6);
		m_octahedron->m_indices.set(indices, 24);
		m_octahedron->GenerateTangents();
		//
		m_octahedron->Bind();
	}

	void Geometry::CreateIcosahedron()
	{
		m_icosahedron = GenerateIcosahdron("Icosahedron [Radius=0.5]", 0, 0.5f);
	}

	void Geometry::CreateIcosphere()
	{
		m_icoSphere = GenerateIcosahdron("Icosphere [Radius=0.5]", 1, 0.5f);
	}

	void Geometry::CreateSphere()
	{
		m_sphere = GenerateIcosahdron("Sphere [Radius=0.5]", 2, 0.5f);
	}

	void Geometry::CreateSphereUV()
	{
		m_sphereUV_16 = GenerateSphereUV("UVSphere16 [Radius= [Radius=0.5]]", 16, 16);
		m_sphereUV_64 = GenerateSphereUV("UVSphere64 [Radius= [Radius=0.5]]", 64, 64);
	}

	void Geometry::CreateCylinders()
	{
		//
		m_cylinder_x = GenerateCylinder("CylinderX [16] [Radius=0.5]", Axis::X, 16, 1.0f, 0.5f, 0.5f);
		m_cylinder_y = GenerateCylinder("CylinderY [16] [Radius=0.5]", Axis::Y, 16, 1.0f, 0.5f, 0.5f);
		m_cylinder_z = GenerateCylinder("CylinderZ [16] [Radius=0.5]", Axis::Z, 16, 1.0f, 0.5f, 0.5f);
		//
	}

	void Geometry::CreateCones()
	{
		//
		m_cone_x = GenerateCone("Cone X-axis [16]", Axis::X, 16, 1.0f, 0.5f);
		m_cone_y = GenerateCone("Cone Y-axis [16]", Axis::Y, 16, 1.0f, 0.5f);
		m_cone_z = GenerateCone("Cone Z-axis [16]", Axis::Z, 16, 1.0f, 0.5f);
		//
	}

	void Geometry::CreateArrows()
	{
		m_arrow_x = GenerateArrow("ArrowX", Axis::X, 1.0f, Vector3::ZERO);
		m_arrow_y = GenerateArrow("ArrowY", Axis::Y, 1.0f, Vector3::ZERO);
		m_arrow_z = GenerateArrow("ArrowZ", Axis::Z, 1.0f, Vector3::ZERO);
	}
}