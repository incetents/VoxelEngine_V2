// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Mesh;

	class Geometry
	{
	private:
		static Mesh* m_fullQuad;
		static Mesh* m_quad;
		static Mesh* m_cube;
		static Mesh* m_inverseCube;

		static void CreateFullQuad();
		static void CreateQuad();
		static void CreateCube();
		static void CreateInverseCube();
	public:
		static void Setup()
		{
			if (!m_fullQuad)
				CreateFullQuad();

			if (!m_quad)
				CreateQuad();

			if (!m_cube)
				CreateCube();

			if (!m_inverseCube)
				CreateInverseCube();
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
	};
}

