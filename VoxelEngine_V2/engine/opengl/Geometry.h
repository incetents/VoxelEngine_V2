// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include <string>

namespace Vxl
{
	class Vector2;
	class Vector3;
	class Mesh;
	enum class Axis;

	static class Geometry : public Singleton<class Geometry>
	{
	private:
		// Vertex Creation
		void CreateCylinder(
			std::vector<Vector3>& pos, std::vector<Vector2>& uvs,
			Axis axis, UINT slices, float height, float radius_top, float radius_bot,
			const Vector3& offset
		);
		void CreateCone(
			std::vector<Vector3>& pos, std::vector<Vector2>& uvs,
			Axis axis, UINT slices, float height, float radius,
			const Vector3& offset
		);

		// Mesh Creation
		Mesh* GenerateIcosahdron(const std::string& MeshName, unsigned int subdivisions, float scale = 1.0f);
		Mesh* GenerateSphereUV(const std::string& MeshName, unsigned int xSlice, unsigned int ySlice);
		Mesh* GenerateCylinder(const std::string& MeshName, Axis axis, UINT slices, float height, float radius_top, float radius_bot);
		Mesh* GenerateCone(const std::string& MeshName, Axis axis, UINT slices, float height, float radius);
		Mesh* GenerateArrow(const std::string& MeshName, Axis axis, float tailLength, const Vector3& offset);

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
		Mesh* m_cylinder_x = nullptr; //
		Mesh* m_cylinder_y = nullptr; //
		Mesh* m_cylinder_z = nullptr; //
		Mesh* m_cone_x = nullptr; //
		Mesh* m_cone_y = nullptr; //
		Mesh* m_cone_z = nullptr; //
		Mesh* m_arrow_x = nullptr;
		Mesh* m_arrow_y = nullptr;
		Mesh* m_arrow_z = nullptr;

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
		void CreateCylinders();
		void CreateCones();
		void CreateArrows();

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
			CreateCylinders();
			CreateCones();
			CreateArrows();
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
		// Vertices are from -0.5 to +0.5
		Mesh* GetIcosahedron()
		{
			return m_icosahedron;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetIcoSphere()
		{
			return m_icoSphere;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetSphere()
		{
			return m_sphere;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetSphereUV_Cheap()
		{
			return m_sphereUV_16;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetSphereUV_Good()
		{
			return m_sphereUV_64;
		}
		Mesh* GetCylinderX()
		{
			return m_cylinder_x;
		}
		Mesh* GetCylinderY()
		{
			return m_cylinder_y;
		}
		Mesh* GetCylinderZ()
		{
			return m_cylinder_z;
		}
		Mesh* GetConeX()
		{
			return m_cone_x;
		}
		Mesh* GetConeY()
		{
			return m_cone_y;
		}
		Mesh* GetConeZ()
		{
			return m_cone_z;
		}
		Mesh* GetArrowX()
		{
			return m_arrow_x;
		}
		Mesh* GetArrowY()
		{
			return m_arrow_y;
		}
		Mesh* GetArrowZ()
		{
			return m_arrow_z;
		}

	} SingletonInstance(Geometry);
}

