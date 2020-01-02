// Copyright (c) 2020 Emmanuel Lajeunesse
#pragma once

#include "Vector.h"

namespace Vxl
{
	class Matrix4x4;

	struct AABB
	{
		Vector3 min;
		Vector3 max;

		AABB()
		{
			min = Vector3::ZERO;
			max = Vector3::ZERO;
		}
		AABB(Vector3 _min, Vector3 _max)
		{
			min = _min;
			max = _max;
		}
	};

	struct OBB
	{
		Vector3 position;
		Vector3 right;
		Vector3 up;
		Vector3 forward;

		// Generate all 8 world space points for the OBB
		std::vector<Vector3> generatePoints();

		// Generate AABB from OBB points
		AABB generateAABB();

		OBB()
		{
			position = Vector3::ZERO;
			right = Vector3::ZERO;
			up = Vector3::ZERO;
			forward = Vector3::ZERO;
		}
		OBB(Vector3 _position, Vector3 _right, Vector3 _up, Vector3 _forward)
		{
			position = _position;
			right = _right;
			up = _up;
			forward = _forward;
		}
		OBB(const Matrix4x4& model, const Vector3& right, const Vector3& up, const Vector3& forward, const Vector3& meshMin, const Vector3& meshMax);
	};

	class Plane
	{
	public:
		Vector3 m_normal;
		float	m_distance;

		Plane(const Vector3& _normal, float _distance)
			: m_normal(_normal), m_distance(_distance)
		{}
		Plane(const Vector3& _normal, const Vector3& _worldPosition)
			: m_normal(_normal), m_distance(_worldPosition.ProjectLength(_normal))
		{}
	};

	struct Ray
	{
		Vector3 m_origin;
		Vector3 m_direction;
		float   m_t = 0;

		Ray(const Vector3& _origin, const Vector3& _direction)
			: m_origin(_origin), m_direction(_direction)
		{}
	};

	struct RayHit
	{
		Vector3 m_location;
		float	m_distance;
		bool	m_missed;
	};

	// Finds where a Ray hits a plane
	RayHit Intersection(const Ray& _ray, const Plane& _plane);

	// Finds the shortest distance between two lines
	float ShortestDistance(Ray& _ray1, Ray& _ray2);
}