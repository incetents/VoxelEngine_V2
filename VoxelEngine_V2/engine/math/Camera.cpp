// Copyright(c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Camera.h"

#include "Vector3.h"
#include <iostream>

namespace Vxl
{
	Camera* Camera::m_main = nullptr;

	Camera::Camera(const Vector3& _position, const Vector3& _forward, float _znear, float _zfar)
	{
		// Data
		setPosition(_position);
		setForward(_forward.Normalize());

		// Projection Empty
		m_projection = new CameraProjection(_znear, _zfar);
		// View
		update();
	}

	void Camera::update()
	{
		m_view = Matrix4x4::LookAt(Transform::getPosition(), Transform::getPosition() + Transform::getForward(), Vector3::UP);
		m_viewInverse = m_view.Inverse();

		m_viewProjection = m_view * m_projection->getProjection();
		m_viewProjectionInverse = m_viewProjection.Inverse();
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
			assert(dynamic_cast<CameraProjection_Perspective*>(m_projection));
			m_projection->Set(_fov, _aspect);
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
			assert(dynamic_cast<CameraProjection_Orthographic*>(m_projection));
			m_projection->Set(_xmin, _xmax, _ymin, _ymax);;
		}

		return *this;
	}
	Camera& Camera::updatePerspective(float _fov, float _aspect)
	{
		m_projection->Update_FovAspect(_fov, _aspect);
		return *this;
	}
	Camera& Camera::updateOrtho_X(float _xmin, float _xmax)
	{
		m_projection->Update_X(_xmin, _xmax);
		return *this;
	}
	Camera& Camera::updateOrtho_Y(float _ymin, float _ymax)
	{
		m_projection->Update_Y(_ymin, _ymax);
		return *this;
	}
	Camera& Camera::setZnear(float _znear)
	{
		m_projection->m_Znear = _znear;
		m_projection->Update_Z();
		return *this;
	}
	Camera& Camera::setZfar(float _zfar)
	{
		m_projection->m_Zfar = _zfar;
		m_projection->Update_Z();
		return *this;
	}

	Transform& Camera::setForward(const Vector3& forward)
	{
		return Transform::setForward(-forward);
	}
}