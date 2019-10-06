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
		Mesh* m_circleUnit_x = nullptr; // Radius = 0.5f
		Mesh* m_circleUnit_y = nullptr; // Radius = 0.5f
		Mesh* m_circleUnit_z = nullptr; // Radius = 0.5f
		Mesh* m_circleDouble_x = nullptr; // Radius = 1.0f
		Mesh* m_circleDouble_y = nullptr; // Radius = 1.0f
		Mesh* m_circleDouble_z = nullptr; // Radius = 1.0f
		Mesh* m_doughtnut2D_x = nullptr; // Radius = 1.0f
		Mesh* m_doughtnut2D_y = nullptr; // Radius = 1.0f
		Mesh* m_doughtnut2D_z = nullptr; // Radius = 1.0f

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
		void CreateCircles();

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
			CreateCircles();
		}

		// Generate Custom Meshes
		Mesh* GenerateIcosahdron(const std::string& MeshName, unsigned int subdivisions, float scale = 1.0f);
		Mesh* GenerateSphereUV(const std::string& MeshName, unsigned int xSlice, unsigned int ySlice);
		Mesh* GenerateCylinder(const std::string& MeshName, Axis axis, UINT slices, float height, float radius_top, float radius_bot);
		Mesh* GenerateCone(const std::string& MeshName, Axis axis, UINT slices, float height, float radius);
		Mesh* GenerateArrow(const std::string& MeshName, Axis axis, float tailLength, const Vector3& offset);
		Mesh* GenerateCube(const std::string& MeshName, float unitSize = 1.0f, const Vector3& offset = Vector3(0, 0, 0));
		Mesh* GenerateQuad(const std::string& MeshName, Axis axis, float unitSize = 1.0f, const Vector3& offset = Vector3(0,0,0));
		Mesh* GenerateCircle(const std::string& MeshName, Axis axis, uint32_t vertices = 32, float unitSize = 1.0f);
		Mesh* GenerateDoughtnut2D(const std::string& MeshName, Axis axis, uint32_t edgeVertices = 32, float exteriorUnitSize = 1.0f, float interiorUnitSize = 1.0f);

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
		// 1/4 the size of a cube [-0.125] to [0.125]
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
		Mesh* GetCircleX_Unit()
		{
			return m_circleUnit_x;
		}
		Mesh* GetCircleY_Unit()
		{
			return m_circleUnit_y;
		}
		Mesh* GetCircleZ_Unit()
		{
			return m_circleUnit_z;
		}
		Mesh* GetCircleX_Double()
		{
			return m_circleDouble_x;
		}
		Mesh* GetCircleY_Double()
		{
			return m_circleDouble_y;
		}
		Mesh* GetCircleZ_Double()
		{
			return m_circleDouble_z;
		}
		Mesh* GetDoughtnutX_2D()
		{
			return m_doughtnut2D_x;
		}
		Mesh* GetDoughtnutY_2D()
		{
			return m_doughtnut2D_y;
		}
		Mesh* GetDoughtnutZ_2D()
		{
			return m_doughtnut2D_z;
		}

	} SingletonInstance(Geometry);
}

