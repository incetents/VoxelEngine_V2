// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Camera.h"

#include "../rendering/RenderManager.h"
#include "../window/window.h"
#include "../rendering/Mesh.h"
#include "../rendering/Geometry.h"
#include "../rendering/Debug.h"

namespace Vxl
{
	// Projection Matrix Creator (based on stored data)
	void Camera::CreatePerspective()
	{
		m_projection = Matrix4x4::Perspective(m_buffer[0], m_buffer[1], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::PerspectiveInverse(m_buffer[0], m_buffer[1], m_znear, m_zfar);
		
		UpdateViewProjection();
	}
	// Projection Matrix Creator (based on stored data)
	void Camera::CreateOrthographic()
	{
		m_projection = Matrix4x4::Orthographic(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::OrthographicInverse(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
		
		UpdateViewProjection();
	}

	void Camera::UpdateFOV(float _fov)
	{
		VXL_ASSERT(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT, "Invalid Type for Camera");
		m_buffer[0] = _fov;
	}
	void Camera::UpdateAspect(float _aspect)
	{
		VXL_ASSERT(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT, "Invalid Type for Camera");
		m_buffer[1] = _aspect;
	}
	void Camera::UpdateOrthoX(float _xmin, float _xmax)
	{
		m_buffer[0] = _xmin;
		m_buffer[1] = _xmax;
	}
	void Camera::UpdateOrthoY(float _ymin, float _ymax)
	{
		m_buffer[2] = _ymin;
		m_buffer[3] = _ymax;
	}

	Camera::Camera(const std::string& name, float _znear, float _zfar)
		: SceneNode(SceneNodeType::CAMERA, name), m_znear(_znear), m_zfar(_zfar)
	{
		//RenderManager.AddEntity(this);

		// Better method of rotating a Camera
		//m_transform.m_rotationOrder = EulerRotationOrder::YXZ;
	}

	Camera::~Camera()
	{
		//RenderManager.RemoveEntity(this);
	}

	void Camera::update()
	{
		m_view = Matrix4x4::LookAt(m_transform.m_position, m_transform.getForward(), m_transform.getRight(), m_transform.getUp());
		m_viewInverse = m_view.Inverse();
		
		UpdateViewProjection();
	}

	//	Camera* Camera::Create(const std::string& _name, float _znear, float _zfar)
	//	{
	//		// Create Unique Name
	//		auto UniqueName = FixNameDuplicate(_name);
	//	
	//		// Create
	//		Camera* _entity = new Camera(UniqueName, _znear, _zfar);
	//		// Store in entity database
	//		AddNamedAsset(UniqueName, _entity, AssetMessage::CREATED);
	//		// Return
	//		return _entity;
	//	}

	// ScreenSpace / WorldSpace conversion
	Vector4 Camera::ScreenSpaceToWorldSpace(const Vector3& screenSpace)
	{
		Vector4 Point = m_viewProjectionInverse * Vector4(screenSpace, 1);
		// Divide all values by its W
		Point.w = 1.0f / Point.w;
		Point.x *= Point.w;
		Point.y *= Point.w;
		Point.z *= Point.w;

		return Point;
	}
	Vector3 Camera::ScreenSpaceToDirection(const Vector2& screenSpace)
	{
		Vector4 EndPoint = m_viewProjectionInverse * Vector4(screenSpace, 1, 1);
		// Divide all values by its W
		EndPoint.w = 1.0f / EndPoint.w;
		EndPoint.x *= EndPoint.w;
		EndPoint.y *= EndPoint.w;
		EndPoint.z *= EndPoint.w;

		return (Vector3(EndPoint) - m_transform.getWorldPosition()).Normalize();
	}
	Vector4 Camera::WorldSpaceToScreenSpace(const Vector3& worldSpace)
	{
		Vector4 Point = m_viewProjection * Vector4(worldSpace, 1);
		// Divide all values by its W
		Point.w = 1.0f / Point.w;
		Point.x *= Point.w;
		Point.y *= Point.w;
		Point.z *= Point.w;

		return Point;
	}

	// Set Type
	void Camera::SetPerspectiveWindowAspect(float _fov)
	{
		m_type = CameraType::PERSPECTIVE_WINDOW_ASPECT;

		UpdateFOV(_fov);
		UpdateAspect(Window.GetAspectRatio());

		CreatePerspective();
	}
	void Camera::SetPerspective(float _fov, float _aspect)
	{
		m_type = CameraType::PERSPECTIVE;

		UpdateFOV(_fov);
		UpdateAspect(_aspect);

		CreatePerspective();
	}
	void Camera::SetOrthographic(float _xmin, float _xmax, float _ymin, float _ymax)
	{
		m_type = CameraType::ORTHOGRAPHIC;

		UpdateOrthoX(_xmin, _xmax);
		UpdateOrthoY(_ymin, _ymax);

		CreateOrthographic();
	}
	void Camera::SetCustom(const Matrix4x4& projection)
	{
		m_type = CameraType::CUSTOM;

		m_projection = projection;
		m_projectionInverse = projection.Inverse();
	}

}