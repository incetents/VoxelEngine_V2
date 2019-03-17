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
	void Geometry::Destroy()
	{
		if (!m_isSetup)
			return;

		delete m_fullQuad;
		delete m_fullTriangle;
		delete m_quad;
		delete m_cube;
		delete m_inverseCube;
		delete m_octahedron;
		delete m_icosahedron;
		delete m_icoSphere;
		delete m_sphere;
		delete m_sphereUV_16;
		delete m_sphereUV_64;
	}

	// Generators
	Mesh* Geometry::GenerateIcosahdron(unsigned int subdivisions)
	{
		float t = (1.0f + sqrt(5.0f)) / 2.0f;

		std::vector<Vector3> m_points;

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
		std::vector<Vector2> uvs;
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

			vertices.push_back(Tri[0]);
			vertices.push_back(Tri[1]);
			vertices.push_back(Tri[2]);

			uvs.push_back(UVs[0]);
			uvs.push_back(UVs[1]);
			uvs.push_back(UVs[2]);
		}

		Mesh* NewMesh = new Mesh("Icosahedron" + std::to_string(subdivisions));
		//
		NewMesh->m_positions.set(vertices);
		NewMesh->m_uvs.set(uvs);
		//
		NewMesh->Bind();
		return NewMesh;
	}
	Mesh* Geometry::GenerateSphereUV(unsigned int xSlice, unsigned int ySlice)
	{
		std::vector<Vector3> positions;
		std::vector<Vector2> uvs;
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

				vec3 blPos = vec3(s0 * sinf((i + 0) * TWO_PI / x), c0, s0 * cosf((i + 0) * TWO_PI / x));
				vec3 brPos = vec3(s0 * sinf((i + 1) * TWO_PI / x), c0, s0 * cosf((i + 1) * TWO_PI / x));
				vec3 tlPos = vec3(s1 * sinf((i + 0) * TWO_PI / x), c1, s1 * cosf((i + 0) * TWO_PI / x));
				vec3 trPos = vec3(s1 * sinf((i + 1) * TWO_PI / x), c1, s1 * cosf((i + 1) * TWO_PI / x));

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

		Mesh* _mesh = new Mesh("Sphere" + std::to_string(xSlice) + '|' + std::to_string(ySlice));
		//
		_mesh->m_positions.set(positions);
		_mesh->m_uvs.set(uvs);
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
		Vector3 normals[] = {
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1)
		};
		GLuint indices[] = {0, 1, 2, 0, 2, 3};

		m_fullQuad = new Mesh("FullQuad");
		//
		m_fullQuad->m_positions.set(pos, 4);
		m_fullQuad->m_uvs.set(uvs, 4);
		m_fullQuad->m_normals.set(normals, 4);
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
		Vector3 normals[] = {
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1)
		};
		GLuint indices[] = { 0, 1, 2 };

		m_fullTriangle = new Mesh("FullTriangle");
		//
		m_fullTriangle->m_positions.set(pos, 3);
		m_fullTriangle->m_uvs.set(uvs, 3);
		m_fullTriangle->m_normals.set(normals, 3);
		//m_fullTriangle->m_indices.set(indices, 6);
		//
		m_fullTriangle->Bind();
	}

	void Geometry::CreateQuad()
	{
		Vector3 pos[] = {
			Vector3(-0.5f, -0.5f, +0.0f),
			Vector3(+0.5f, -0.5f, +0.0f),
			Vector3(+0.5f, +0.5f, +0.0f),
			Vector3(-0.5f, +0.5f, +0.0f),
		};
		Vector2 uvs[] = {
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1),
			Vector2(0, 1)
		};
		Vector3 normals[] = {
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1),
			Vector3(0, 0, +1)
		};
		GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

		m_quad = new Mesh("Quad");
		//
		m_quad->m_positions.set(pos, 4);
		m_quad->m_uvs.set(uvs, 4);
		m_quad->m_normals.set(normals, 4);
		m_quad->m_indices.set(indices, 6);
		//
		m_quad->Bind();
	}

	void Geometry::CreateCube()
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

		m_cube = new Mesh("Cube");
		//
		m_cube->m_positions.set(pos, 24);
		m_cube->m_uvs.set(uvs, 24);
		m_cube->m_normals.set(normals, 24);
		m_cube->m_indices.set(indices, 36);

		//
		m_cube->Bind();
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

		m_inverseCube = new Mesh("InverseCube");
		//
		m_inverseCube->m_positions.set(pos, 24);
		m_inverseCube->m_uvs.set(uvs, 24);
		m_inverseCube->m_normals.set(normals, 24);
		m_inverseCube->m_indices.set(indices, 36);
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

		m_octahedron = new Mesh("Octahedron");
		//
		m_octahedron->m_positions.set(pos, 6);
		m_octahedron->m_uvs.set(uvs, 6);
		m_octahedron->m_normals.set(normals, 6);
		m_octahedron->m_indices.set(indices, 24);
		//
		m_octahedron->Bind();
	}

	void Geometry::CreateIcosahedron()
	{
		m_icosahedron = GenerateIcosahdron(0);
	}

	void Geometry::CreateIcosphere()
	{
		m_icoSphere = GenerateIcosahdron(1);
	}

	void Geometry::CreateSphere()
	{
		m_sphere = GenerateIcosahdron(2);
	}

	void Geometry::CreateSphereUV()
	{
		m_sphereUV_16 = GenerateSphereUV(16, 16);
		m_sphereUV_64 = GenerateSphereUV(64, 64);
	}
}