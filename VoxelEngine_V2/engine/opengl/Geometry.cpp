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
	// Shapes
	Mesh* Geometry::m_fullQuad = nullptr;
	Mesh* Geometry::m_quad = nullptr;
	Mesh* Geometry::m_cube = nullptr;
	Mesh* Geometry::m_inverseCube = nullptr;

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
}