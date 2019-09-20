// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Raycast.h"

namespace Vxl
{
	RayHit Intersection(const Ray& _ray, const Plane& _plane)
	{
		RayHit		hit;
		Vector3		plane_origin =	_plane.m_normal * _plane.m_distance;
		float		denom =			_plane.m_normal.Dot(_ray.m_direction);
		if (fabs(denom) > 1e-6)
		{
			hit.m_distance = _plane.m_normal.Dot(plane_origin - _ray.m_origin) / denom;
			hit.m_location = _ray.m_origin + _ray.m_direction * hit.m_distance;
			hit.m_hit = true;
		}
		else
		{
			hit.m_distance = 0.0f;
			hit.m_location = Vector3::ZERO;
			hit.m_hit = false;
		}

		return hit;
	}
}