// Copyright(c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Camera.h"

#include "Vector3.h"
#include <iostream>

namespace Vxl
{
	Camera::Camera(const Vector3& _position, const Vector3& _forward, float _znear, float _zfar)
	{
		// Data
		m_transform.setPosition(_position);
		m_transform.setForward(_forward);

		// View
		update();
		// Projection Empty
		m_projection = new CameraProjection(_znear, _zfar);
	}

	void Camera::update()
	{
		m_view.LookAt(m_transform.getPosition(), m_transform.getPosition() - m_transform.getForward(), m_transform.getUp());
		m_viewInverse = m_view.Inverse();
	}

	Camera& Camera::setPerspective(float _fov, float _aspect)
	{
		float zNear = getZnear();
		float zFar = getZfar();
		if (m_type != Type::PERSPECTIVE)
		{
			delete m_projection;
			m_projection = new CameraProjection_Perspective(_fov, _aspect, zNear, zFar);
			m_type = Type::PERSPECTIVE;
		}
		else
		{
			CameraProjection_Perspective* pp = dynamic_cast<CameraProjection_Perspective*>(m_projection);
#if _DEBUG
			assert(pp);
#endif
			pp->Set(_fov, _aspect);
		}

		return *this;
	}
	Camera& Camera::setOrthographic(float _xmin, float _xmax, float _ymin, float _ymax)
	{
		float zNear = getZnear();
		float zFar = getZfar();
		if (m_type != Type::ORTHOGRAPHIC)
		{
			delete m_projection;
			m_projection = new CameraProjection_Orthographic(_xmin, _xmax, _ymin, _ymax, zNear, zFar);
			m_type = Type::ORTHOGRAPHIC;
		}
		else
		{
			CameraProjection_Orthographic* po = dynamic_cast<CameraProjection_Orthographic*>(m_projection);
#if _DEBUG
			assert(po);
#endif
			po->Set(_xmin, _xmax, _ymin, _ymax);;
		}
		
		return *this;
	}
}