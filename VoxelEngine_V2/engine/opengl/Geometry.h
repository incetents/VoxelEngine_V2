// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"

namespace Vxl
{
	class Mesh;

	static class Geometry : public Singleton<class Geometry>
	{
	private:
		// Special Utility
		Mesh* GenerateIcosahdron(unsigned int subdivisions, float scale = 1.0f);
		Mesh* GenerateSphereUV(unsigned int xSlice, unsigned int ySlice);

	private:
		bool m_isSetup = false;
		Mesh* m_fullQuad = nullptr;
		Mesh* m_fullTriangle = nullptr;
		Mesh* m_quad = nullptr;
		Mesh* m_cube = nullptr;
		Mesh* m_inverseCube = nullptr;
		Mesh* m_octahedron = nullptr;
		Mesh* m_icosahedron = nullptr;
		Mesh* m_icoSphere = nullptr;
		Mesh* m_sphere = nullptr; // made from icosahedron subdivded
		Mesh* m_spherehalf = nullptr;
		Mesh* m_sphereUV_16 = nullptr;
		Mesh* m_sphereUV_64 = nullptr;

		void CreateFullQuad();
		void CreateFullTriangle();
		void CreateQuad();
		void CreateCube();
		void CreateInverseCube();
		void CreateOctahedron();
		void CreateIcosahedron();
		void CreateIcosphere();
		void CreateSphere();
		void CreateSphereUV();

	public:
		void Setup()
		{
			CreateFullQuad();
			CreateFullTriangle();
			CreateQuad();
			CreateCube();
			CreateInverseCube();
			CreateOctahedron();
			CreateIcosahedron();
 			CreateIcosphere();
			CreateSphere();
			CreateSphereUV();
		}

		Mesh* GetFullQuad()
		{
			return m_fullQuad;
		}
		Mesh* GetFullTriangle()
		{
			return m_fullTriangle;
		}
		Mesh* GetQuad()
		{
			return m_quad;
		}
		Mesh* GetCube()
		{
			return m_cube;
		}
		Mesh* GetInverseCube()
		{
			return m_inverseCube;
		}
		Mesh* GetOctahedron()
		{
			return m_octahedron;
		}
		// Vertices are from -1 to +1
		Mesh* GetIcosahedron()
		{
			return m_icosahedron;
		}
		// Vertices are from -1 to +1
		Mesh* GetIcoSphere()
		{
			return m_icoSphere;
		}
		// Vertices are from -1 to +1
		Mesh* GetSphere()
		{
			return m_sphere;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetSphereHalf()
		{
			return m_spherehalf;
		}
		// Vertices are from -1 to +1
		Mesh* GetSphereUV_Cheap()
		{
			return m_sphereUV_16;
		}
		// Vertices are from -1 to +1
		Mesh* GetSphereUV_Good()
		{
			return m_sphereUV_64;
		}

	} SingletonInstance(Geometry);
}

