// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Collision.h"

#undef max

namespace Vxl
{
	std::vector<Vector3> OBB::generatePoints()
	{
		// All combinations of
		// Vec3(center  +,-  right  +,-  up  +,-  forward)
		// in a vector

		Vector3 hRight = right * 0.5f;
		Vector3 hUp = up * 0.5f;
		Vector3 hForward = forward * 0.5f;

		Vector3 pos_right = position + hRight;
		Vector3 pos_left = position - hRight;
		Vector3 up_forward = hUp + hForward;
		Vector3 up_back = hUp - hForward;

		std::vector<Vector3> positions = {
			Vector3(pos_left - up_back),
			Vector3(pos_right - up_back),
			Vector3(pos_left + up_back),
			Vector3(pos_right + up_back),
			Vector3(pos_left - up_forward),
			Vector3(pos_right - up_forward),
			Vector3(pos_left + up_forward),
			Vector3(pos_right + up_forward)
		};

		return positions;
	}

	AABB OBB::generateAABB()
	{
		AABB aabb(
			Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
			Vector3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
		);
		std::vector<Vector3> positions = generatePoints();
		for (const auto& pos : positions)
		{
			aabb.min = Vector3::Min(aabb.min, pos);
			aabb.max = Vector3::Max(aabb.max, pos);
		}
		return aabb;
	}

	RayHit Intersection(const Ray& _ray, const Plane& _plane)
	{
		RayHit		hit;
		Vector3		plane_origin = _plane.m_normal * _plane.m_distance;
		float		denom = _plane.m_normal.Dot(_ray.m_direction);
		if (fabs(denom) > 1e-6)
		{
			hit.m_distance = _plane.m_normal.Dot(plane_origin - _ray.m_origin) / denom;
			hit.m_location = _ray.m_origin + _ray.m_direction * hit.m_distance;
			hit.m_missed = false;
		}
		else
		{
			hit.m_distance = 0.0f;
			hit.m_location = Vector3::ZERO;
			hit.m_missed = true;
		}

		return hit;
	}

	// Finds the shortest distance between two lines
	float ShortestDistance(Ray& _ray1, Ray& _ray2)
	{
		Vector3 dp = _ray2.m_origin - _ray1.m_origin;

		const float v12 = Vector3::Dot(_ray1.m_direction, _ray1.m_direction);
		const float v22 = Vector3::Dot(_ray2.m_direction, _ray2.m_direction);
		const float v1v2 = Vector3::Dot(_ray1.m_direction, _ray2.m_direction);

		const float det = v1v2 * v1v2 - v12 * v22;

		if (std::abs(det) > FLT_MIN)
		{
			const float inv_det = 1.f / det;

			const float dpv1 = Vector3::Dot(dp, _ray1.m_direction);
			const float dpv2 = Vector3::Dot(dp, _ray2.m_direction);

			_ray1.m_t = inv_det * (v22 * dpv1 - v1v2 * dpv2);
			_ray2.m_t = inv_det * (v1v2 * dpv1 - v12 * dpv2);

			return (dp + _ray2.m_direction * _ray2.m_t - _ray1.m_direction * _ray1.m_t).Length();
		}
		else
		{
			const Vector3 a = Vector3::Cross(dp, _ray1.m_direction);
			return std::sqrt(Vector3::Dot(a, a) / v12);
		}
	}
}