// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "CameraObject.h"

#include "../modules/RenderManager.h"
#include "../window/window.h"

namespace Vxl
{
	void CameraObject::BindUBO()
	{
		m_UBO.sendMatrix(m_viewProjection, 0);
		m_UBO.sendMatrix(m_view, 64);
		m_UBO.sendMatrix(m_projection, 128);
	}

	// Projection Matrix Creator (based on stored data)
	void CameraObject::CreatePerspective()
	{
		m_projection = Matrix4x4::Perspective(m_buffer[0], m_buffer[1], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::PerspectiveInverse(m_buffer[0], m_buffer[1], m_znear, m_zfar);
	}
	// Projection Matrix Creator (based on stored data)
	void CameraObject::CreateOrthographic()
	{
		m_projection = Matrix4x4::Orthographic(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::OrthographicInverse(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
	}

	void CameraObject::UpdateFOV(float _fov)
	{
		assert(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT);
		m_buffer[0] = _fov;
	}
	void CameraObject::UpdateAspect(float _aspect)
	{
		assert(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT);
		m_buffer[1] = _aspect;
	}
	void CameraObject::UpdateOrthoX(float _xmin, float _xmax)
	{
		m_buffer[0] = _xmin;
		m_buffer[1] = _xmax;
	}
	void CameraObject::UpdateOrthoY(float _ymin, float _ymax)
	{
		m_buffer[2] = _ymin;
		m_buffer[3] = _ymax;
	}

	CameraObject::CameraObject(const std::string& name, float _znear, float _zfar)
		: Entity(name, EntityType::CAMERA), m_znear(_znear), m_zfar(_zfar)
	{
		m_transform.m_reversedForward = true;

		RenderManager.AddEntity(this);
	}

	CameraObject::~CameraObject()
	{
		RenderManager.RemoveEntity(this);
	}

	CameraObject* CameraObject::Create(const std::string& _name, float _znear, float _zfar)
	{
		// Create Unique Name
		auto UniqueName = FixNameDuplicate(_name);

		// Create
		CameraObject* _entity = new CameraObject(UniqueName, _znear, _zfar);
		// Store in entity database
		AddToDatabase(UniqueName, _entity);
		Message_Created(UniqueName, _entity);
		// Return
		return _entity;
	}

	// Set Type
	void CameraObject::SetPerspectiveWindowAspect(float _fov)
	{
		m_type = CameraType::PERSPECTIVE_WINDOW_ASPECT;

		UpdateFOV(_fov);
		UpdateAspect(Window.GetAspectRatio());

		CreatePerspective();
	}
	void CameraObject::SetPerspective(float _fov, float _aspect)
	{
		m_type = CameraType::PERSPECTIVE;

		UpdateFOV(_fov);
		UpdateAspect(_aspect);

		CreatePerspective();
	}
	void CameraObject::SetOrthographic(float _xmin, float _xmax, float _ymin, float _ymax)
	{
		m_type = CameraType::ORTHOGRAPHIC;

		UpdateOrthoX(_xmin, _xmax);
		UpdateOrthoY(_ymin, _ymax);

		CreateOrthographic();
	}
	void CameraObject::SetCustom(const Matrix4x4& projection)
	{
		m_type = CameraType::CUSTOM;

		m_projection = projection;
		m_projectionInverse = projection.Inverse();
	}

	// Behaviour
	void CameraObject::Update()
	{
		m_transform.updateValues();
		m_view = Matrix4x4::LookAt(m_transform.m_position, m_transform.m_position + m_transform.m_forward, Vector3::UP);
		m_viewInverse = m_view.Inverse();

		m_viewProjection = m_view * m_projection;
		m_viewProjectionInverse = m_viewProjection.Inverse();
	}

	void CameraObject::Draw()
	{

	}

	void CameraObject::TransformChanged()
	{
	}
}