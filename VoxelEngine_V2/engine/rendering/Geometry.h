// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../utilities/singleton.h"
#include <string>

namespace Vxl
{
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
		Mesh* GenerateCube(const std::string& MeshName, float unitSize = 1.0f);
		Mesh* GenerateQuad(const std::string& MeshName, Axis axis, float unitSize = 1.0f);

	private:
		bool m_isSetup = false;
		Mesh* m_fullQuad = nullptr;
		Mesh* m_fullTriangle = nullptr;
		Mesh* m_quad_x = nullptr;
		Mesh* m_quad_y = nullptr;
		Mesh* m_quad_z = nullptr;
		Mesh* m_halfquad_x = nullptr;
		Mesh* m_halfquad_y = nullptr;
		Mesh* m_halfquad_z = nullptr;
		Mesh* m_cube = nullptr;
		Mesh* m_cube_small = nullptr; // (1/4)size
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
		void InitGLResources()
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

		// Vertices are from -1 to +1
		Mesh* GetFullQuad()
		{
			return m_fullQuad;
		}
		Mesh* GetFullTriangle()
		{
			return m_fullTriangle;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetQuadX()
		{
			return m_quad_x;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetQuadY()
		{
			return m_quad_y;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetQuadZ()
		{
			return m_quad_z;
		}
		// Vertices are from -0.25 to +0.25
		Mesh* GetHalfQuadX()
		{
			return m_halfquad_x;
		}
		// Vertices are from -0.25 to +0.25
		Mesh* GetHalfQuadY()
		{
			return m_halfquad_y;
		}
		// Vertices are from -0.25 to +0.25
		Mesh* GetHalfQuadZ()
		{
			return m_halfquad_z;
		}
		// Vertices are from -0.5 to +0.5
		Mesh* GetCube()
		{
			return m_cube;
		}
		Mesh* GetCubeSmall()
		{
			return m_cube_small;
		}
		// Vertices are from -0.5 to +0.5
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

