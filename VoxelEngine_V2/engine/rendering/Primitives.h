// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector.h"
#include "../utilities/Types.h"
#include "../utilities/singleton.h"
#include <string>

namespace Vxl
{
	class Mesh;
	enum class Axis;

	static class Primitives : public Singleton<class Primitives>
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
		MeshIndex m_fullQuad;
		MeshIndex m_fullTriangle;
		MeshIndex m_quad_x;
		MeshIndex m_quad_y;
		MeshIndex m_quad_z;
		MeshIndex m_halfquad_x;
		MeshIndex m_halfquad_y;
		MeshIndex m_halfquad_z;
		MeshIndex m_cube;
		MeshIndex m_cube_small; // (1/4)size
		MeshIndex m_inverseCube;
		MeshIndex m_octahedron;
		MeshIndex m_icosahedron;
		MeshIndex m_icoSphere;
		MeshIndex m_sphere; // made from icosahedron subdivded
		MeshIndex m_sphereUV_16;
		MeshIndex m_sphereUV_32;
		MeshIndex m_sphereUV_64;
		MeshIndex m_cylinder_x; //
		MeshIndex m_cylinder_y; //
		MeshIndex m_cylinder_z; //
		MeshIndex m_cone_x; //
		MeshIndex m_cone_y; //
		MeshIndex m_cone_z; //
		MeshIndex m_arrow_x;
		MeshIndex m_arrow_y;
		MeshIndex m_arrow_z;
		MeshIndex m_arrow_x_notail;
		MeshIndex m_arrow_y_notail;
		MeshIndex m_arrow_z_notail;
		MeshIndex m_circleUnit_x; // Radius = 0.5f
		MeshIndex m_circleUnit_y; // Radius = 0.5f
		MeshIndex m_circleUnit_z; // Radius = 0.5f
		MeshIndex m_circleDouble_x; // Radius = 1.0f
		MeshIndex m_circleDouble_y; // Radius = 1.0f
		MeshIndex m_circleDouble_z; // Radius = 1.0f
		MeshIndex m_doughtnut2D_x; // Radius = 1.0f
		MeshIndex m_doughtnut2D_y; // Radius = 1.0f
		MeshIndex m_doughtnut2D_z; // Radius = 1.0f

		// Lines
		MeshIndex m_lines_circleX;
		MeshIndex m_lines_circleY;
		MeshIndex m_lines_circleZ;
		MeshIndex m_lines_circleAllAxis;
		MeshIndex m_lines_cube;

	public:
		void InitGLResources();

		// Generate Custom Meshes
		MeshIndex GenerateIcosahdron(const std::string& MeshName, unsigned int subdivisions, float scale = 1.0f);
		MeshIndex GenerateSphereUV(const std::string& MeshName, unsigned int xSlice, unsigned int ySlice);
		MeshIndex GenerateCylinder(const std::string& MeshName, Axis axis, UINT slices, float height, float radius_top, float radius_bot);
		MeshIndex GenerateCone(const std::string& MeshName, Axis axis, UINT slices, float height, float radius);
		MeshIndex GenerateArrow(const std::string& MeshName, Axis axis, float tailLength, const Vector3& offset, bool tail);
		MeshIndex GenerateCube(const std::string& MeshName, float unitSize = 1.0f, const Vector3& offset = Vector3(0, 0, 0));
		MeshIndex GenerateQuad(const std::string& MeshName, Axis axis, float unitSize = 1.0f, const Vector3& offset = Vector3(0,0,0));
		MeshIndex GenerateCircle(const std::string& MeshName, Axis axis, uint32_t vertices = 32, float unitSize = 1.0f);
		MeshIndex GenerateDoughtnut2D(const std::string& MeshName, Axis axis, uint32_t edgeVertices = 32, float exteriorUnitSize = 1.0f, float interiorUnitSize = 1.0f);

		MeshIndex GenerateLinesCircle(const std::string& MeshName, Axis axis, uint32_t vertices = 32, float unitSize = 1.0f);
		MeshIndex GenerateLinesCube(const std::string& MeshName, float unitSize = 1.0f);
		MeshIndex GenerateLinesArrow(const std::string& MeshName, Axis axis, float unitSize = 1.0f);

		// Vertices are from -1 to +1
		inline MeshIndex GetFullQuad() const { return m_fullQuad; }
		inline MeshIndex GetFullTriangle() const { return m_fullTriangle; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetQuadX() const { return m_quad_x; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetQuadY() const { return m_quad_y; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetQuadZ() const { return m_quad_z; }
		// Vertices are from -0.25 to +0.25
		inline MeshIndex GetHalfQuadX() const { return m_halfquad_x; }
		// Vertices are from -0.25 to +0.25
		inline MeshIndex GetHalfQuadY() const { return m_halfquad_y; }
		// Vertices are from -0.25 to +0.25
		inline MeshIndex GetHalfQuadZ() const { return m_halfquad_z; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetCube() const { return m_cube; }
		// 1/4 the size of a cube [-0.125] to [0.125]
		inline MeshIndex GetCubeSmall() const { return m_cube_small; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetInverseCube() const { return m_inverseCube; }
		inline MeshIndex GetOctahedron() const { return m_octahedron; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetIcosahedron() const { return m_icosahedron; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetIcoSphere() const { return m_icoSphere; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetSphere() const { return m_sphere; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetSphereUV_Cheap() const { return m_sphereUV_16; }
		// Vertices are from - 0.5 to + 0.5
		inline MeshIndex GetSphereUV_Medium() const { return m_sphereUV_32; }
		// Vertices are from -0.5 to +0.5
		inline MeshIndex GetSphereUV_Good() const { return m_sphereUV_64; }
		inline MeshIndex GetCylinderX() const { return m_cylinder_x; }
		inline MeshIndex GetCylinderY() const { return m_cylinder_y; }
		inline MeshIndex GetCylinderZ() const { return m_cylinder_z; }
		inline MeshIndex GetConeX() const { return m_cone_x; }
		inline MeshIndex GetConeY() const { return m_cone_y; }
		inline MeshIndex GetConeZ() const { return m_cone_z; }
		inline MeshIndex GetArrowX() const { return m_arrow_x; }
		inline MeshIndex GetArrowY() const { return m_arrow_y; }
		inline MeshIndex GetArrowZ() const { return m_arrow_z; }
		inline MeshIndex GetArrowXNoTail() const { return m_arrow_x_notail; }
		inline MeshIndex GetArrowYNoTail() const { return m_arrow_y_notail; }
		inline MeshIndex GetArrowZNoTail() const { return m_arrow_z_notail; }
		inline MeshIndex GetCircleX_Unit() const { return m_circleUnit_x; }
		inline MeshIndex GetCircleY_Unit() const { return m_circleUnit_y; }
		inline MeshIndex GetCircleZ_Unit() const { return m_circleUnit_z; }
		inline MeshIndex GetCircleX_Double() const { return m_circleDouble_x; }
		inline MeshIndex GetCircleY_Double() const { return m_circleDouble_y; }
		inline MeshIndex GetCircleZ_Double() const { return m_circleDouble_z; }
		inline MeshIndex GetDoughtnutX_2D() const { return m_doughtnut2D_x; }
		inline MeshIndex GetDoughtnutY_2D() const { return m_doughtnut2D_y; }
		inline MeshIndex GetDoughtnutZ_2D() const { return m_doughtnut2D_z; }

		// LINES //

		inline MeshIndex GetLines_CircleX_Unit() const { return m_lines_circleX; }
		inline MeshIndex GetLines_CircleY_Unit() const { return m_lines_circleY; }
		inline MeshIndex GetLines_CircleZ_Unit() const { return m_lines_circleZ; }
		inline MeshIndex GetLines_CircleAllAxis_Unit() const { return m_lines_circleAllAxis; }
		inline MeshIndex GetLines_Cube() const { return m_lines_cube; }

	} SingletonInstance(Primitives);
}

