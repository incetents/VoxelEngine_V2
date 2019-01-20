// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Mesh;

	class Geometry
	{
	private:
		// Special Utility
		static Mesh* GenerateIcosahdron(unsigned int subdivisions);

	private:
		static bool m_isSetup;
		static Mesh* m_fullQuad;
		static Mesh* m_quad;
		static Mesh* m_cube;
		static Mesh* m_inverseCube;
		static Mesh* m_octahedron;
		static Mesh* m_icosahedron;
		static Mesh* m_icoSphere;
		static Mesh* m_sphere;

		static void CreateFullQuad();
		static void CreateQuad();
		static void CreateCube();
		static void CreateInverseCube();
		static void CreateOctahedron();
		static void CreateIcosahedron();
		static void CreateIcosphere();
		static void CreateSphere();

		static void Destroy();
	public:
		static void Setup()
		{
			if (m_isSetup)
				return;

			CreateFullQuad();
			CreateQuad();
			CreateCube();
			CreateInverseCube();
			CreateOctahedron();
			CreateIcosahedron();
			CreateIcosphere();
			CreateSphere();

			m_isSetup = true;
		}
		static void Reload()
		{
			if (!m_isSetup)
				return;

			Destroy();
			m_isSetup = false;
			Setup();
		}

		static Mesh* GetFullQuad()
		{
			return m_fullQuad;
		}
		static Mesh* GetQuad()
		{
			return m_quad;
		}
		static Mesh* GetCube()
		{
			return m_cube;
		}
		static Mesh* GetInverseCube()
		{
			return m_inverseCube;
		}
		static Mesh* GetOctahedron()
		{
			return m_octahedron;
		}
		static Mesh* GetIcosahedron()
		{
			return m_icosahedron;
		}
		static Mesh* GetIcoSphere()
		{
			return m_icoSphere;
		}
		static Mesh* GetSphere()
		{
			return m_sphere;
		}
	};
}

