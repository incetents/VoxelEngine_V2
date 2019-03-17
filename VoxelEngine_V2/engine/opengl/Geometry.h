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
		Mesh* GenerateIcosahdron(unsigned int subdivisions);
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

		void Destroy();
	public:
		void Setup()
		{
			if (m_isSetup)
				return;

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

			m_isSetup = true;
		}
		void Reload()
		{
			if (!m_isSetup)
				return;

			Destroy();
			m_isSetup = false;
			Setup();
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
		Mesh* GetIcosahedron()
		{
			return m_icosahedron;
		}
		Mesh* GetIcoSphere()
		{
			return m_icoSphere;
		}
		Mesh* GetSphere()
		{
			return m_sphere;
		}
		Mesh* GetSphereUV_Cheap()
		{
			return m_sphereUV_16;
		}
		Mesh* GetSphereUV_Good()
		{
			return m_sphereUV_64;
		}

	} SingletonInstance(Geometry);
}

