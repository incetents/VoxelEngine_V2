// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Camera.h"

#include "../opengl/UBO.h"

#include "Vector3.h"

#include <iostream>

namespace Vxl
{
	Database<Camera> Camera::m_database;

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

		// UBO
		static unsigned int CamerasCreated = 0;
		m_UBO = new UniformBufferObject(64 * 3, 0, "Camera");
		CamerasCreated++;
	}
	Camera::~Camera()
	{
		delete m_projection;
		delete m_UBO;
	}

	Camera* Camera::Create(const std::string _name, const Vector3& _position, const Vector3& _forward, float _znear, float _zfar)
	{
		Camera* C = new Camera(_position, _forward, _znear, _zfar);
		m_database.Set(_name, C);
		return C;
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
			m_projection->Set(_xmin, _xmax, _ymin, _ymax);
		}

		return *this;
	}
	Camera& Camera::setCustomProjection(float* _matrix)
	{
		assert(_matrix);

		if (m_type != Type::CUSTOM)
		{
			delete m_projection;
			m_projection = new CameraProjection_Custom(_matrix);
			m_type = Type::CUSTOM;
		}
		else
		{
			assert(dynamic_cast<CameraProjection_Custom*>(m_projection));
			m_projection->Set(_matrix);
		}

		return *this;
	}
	Camera& Camera::updatePerspective(float _fov, float _aspect)
	{
		assert(m_type == Type::PERSPECTIVE);

		m_projection->Update_FovAspect(_fov, _aspect);
		return *this;
	}
	Camera& Camera::updatePerspectiveFOV(float _fov)
	{
		assert(m_type == Type::PERSPECTIVE);

		m_projection->Update_FovAspect(_fov, m_projection->getAspect());
		return *this;
	}
	Camera& Camera::updatePerspectiveAspectRatio(float _aspect)
	{
		assert(m_type == Type::PERSPECTIVE);

		m_projection->Update_FovAspect(m_projection->getFov(), _aspect);
		return *this;
	}
	Camera& Camera::updateOrtho_X(float _xmin, float _xmax)
	{
		assert(m_type == Type::ORTHOGRAPHIC);

		m_projection->Update_X(_xmin, _xmax);
		return *this;
	}
	Camera& Camera::updateOrtho_Y(float _ymin, float _ymax)
	{
		assert(m_type == Type::ORTHOGRAPHIC);

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

	void Camera::BindUBO()
	{
		m_UBO->sendMatrix(m_viewProjection, 0);
		m_UBO->sendMatrix(m_view, 64);
		m_UBO->sendMatrix(m_projection->getProjection(), 128);
	}

	Transform& Camera::setForward(const Vector3& forward)
	{
		return Transform::setForward(-forward);
	}
}