// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Geometry.h"

#include "Mesh.h"

#include "../math/MathCore.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace Vxl
{
	bool Geometry::m_isSetup = false;

	// Shapes
	Mesh* Geometry::m_fullQuad = nullptr;
	Mesh* Geometry::m_quad = nullptr;
	Mesh* Geometry::m_cube = nullptr;
	Mesh* Geometry::m_inverseCube = nullptr;
	Mesh* Geometry::m_octahedron = nullptr;
	Mesh* Geometry::m_icosahedron = nullptr;
	Mesh* Geometry::m_icoSphere = nullptr;
	Mesh* Geometry::m_sphere = nullptr;

	// Setup
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

		m_fullQuad = new Mesh();
		//
		m_fullQuad->m_positions.set(pos, 4);
		m_fullQuad->m_uvs.set(uvs, 4);
		m_fullQuad->m_normals.set(normals, 4);
		m_fullQuad->m_indices.set(indices, 6);
		//
		m_fullQuad->Bind();
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

		m_quad = new Mesh();
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

		m_cube = new Mesh();
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

		m_inverseCube = new Mesh();
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

		m_octahedron = new Mesh();
		//
		m_octahedron->m_positions.set(pos, 6);
		m_octahedron->m_uvs.set(uvs, 6);
		m_octahedron->m_normals.set(normals, 6);
		m_octahedron->m_indices.set(indices, 24);
		//
		m_octahedron->Bind();
	}

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

		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;
		Vec3 Tri[3], Normal;
		Vec2 UV;
		for (GLuint i = 0; i < faceCount; i++)
		{
			Tri[0] = m_points[m_faces[i].x];
			Tri[1] = m_points[m_faces[i].y];
			Tri[2] = m_points[m_faces[i].z];
			
			Vec3 FlatNormal = Vec3::Cross(Tri[0] - Tri[2], Tri[1] - Tri[2]);
			bool UVfix = false;//(FlatNormal.x < 0);

			for (int i = 0; i < 3; i++)
			{
				Vec3 N = Tri[i].Normalize();
				
				UV.x = (std::atan2(N.z, N.x) / PI + 1.f) * .5f;
				UV.y = 0.5f - std::asin(N.y) / PI;

				//if (UVfix && UV)
				if(UV.x < 0.25f && UVfix)
					UV.x += 1.0f;

				vertices.push_back(Tri[i]);
				normals.push_back(N);
				uvs.push_back(UV);
			}
		}

		Mesh* NewMesh = new Mesh();
		//
		NewMesh->m_positions.set(vertices);
		NewMesh->m_uvs.set(uvs);
		NewMesh->m_normals.set(normals);
		//
		NewMesh->Bind();
		return NewMesh;
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
}