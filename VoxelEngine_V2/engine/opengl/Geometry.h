// Copyright (c) 2018 Emmanuel Lajeunesse
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

	private:
		bool m_isSetup = false;
		Mesh* m_fullQuad = nullptr;
		Mesh* m_quad = nullptr;
		Mesh* m_cube = nullptr;
		Mesh* m_inverseCube = nullptr;
		Mesh* m_octahedron = nullptr;
		Mesh* m_icosahedron = nullptr;
		Mesh* m_icoSphere = nullptr;
		Mesh* m_sphere = nullptr;

		void CreateFullQuad();
		void CreateQuad();
		void CreateCube();
		void CreateInverseCube();
		void CreateOctahedron();
		void CreateIcosahedron();
		void CreateIcosphere();
		void CreateSphere();

		void Destroy();
	public:
		void Setup()
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
	} SingletonInstance(Geometry);
}

