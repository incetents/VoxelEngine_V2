// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Vector.h"

namespace Vxl
{
	class Plane
	{
	public:
		Vector3 m_normal;
		float	m_distance;

		Plane(const Vector3& _normal, float _distance)
			: m_normal(_normal), m_distance(_distance)
		{}
	};

	class Ray
	{
	public:
		Vector3 m_origin;
		Vector3 m_direction;

		Ray(const Vector3& _origin, const Vector3& _direction)
			: m_origin(_origin), m_direction(_direction)
		{}
	};

	struct RayHit
	{
		Vector3 m_location;
		float	m_distance;
		bool	m_hit;
	};

	// Formulas
	RayHit Intersection(const Ray& _ray, const Plane& _plane);
	

	
}