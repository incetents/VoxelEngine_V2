// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "CameraObject.h"

#include "../modules/RenderManager.h"
#include "../window/window.h"
#include "../rendering/Mesh.h"
#include "../rendering/Geometry.h"
#include "../rendering/Debug.h"

namespace Vxl
{
	// Projection Matrix Creator (based on stored data)
	void CameraObject::CreatePerspective()
	{
		m_projection = Matrix4x4::Perspective(m_buffer[0], m_buffer[1], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::PerspectiveInverse(m_buffer[0], m_buffer[1], m_znear, m_zfar);
		
		UpdateViewProjection();
	}
	// Projection Matrix Creator (based on stored data)
	void CameraObject::CreateOrthographic()
	{
		m_projection = Matrix4x4::Orthographic(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
		m_projectionInverse = Matrix4x4::OrthographicInverse(m_buffer[0], m_buffer[1], m_buffer[2], m_buffer[3], m_znear, m_zfar);
		
		UpdateViewProjection();
	}

	void CameraObject::UpdateFOV(float _fov)
	{
		VXL_ASSERT(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT, "Invalid Type for CameraObject");
		m_buffer[0] = _fov;
	}
	void CameraObject::UpdateAspect(float _aspect)
	{
		VXL_ASSERT(m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT, "Invalid Type for CameraObject");
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
		AddNamedAsset(UniqueName, _entity, AssetMessage::CREATED);
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
	}

	// Special
	void CameraObject::DrawFrustum(float Width, Color4F Color)
	{
		// Draw Forward
		Vector3 Position = m_transform.getWorldPosition();
		Vector3 Forward = m_transform.getCameraForward();
		//Debug.DrawLine(Position, Position + Forward * 10.0f, 5.0f);

		// Draw Perspective
		if (m_type == CameraType::PERSPECTIVE || m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT)
		{
			// Get ClosePlane size
			float Hnear = 2 * tan(m_buffer[0] / 2) * m_znear;
			float Wnear = Hnear * m_buffer[1];
			Vector3 ClosePlaneCenter = Position + Forward * m_znear;
			Vector3 ClosePlaneHalfUp = m_transform.getUp() * Hnear * 0.5f;
			Vector3 ClosePlaneHalfRight = m_transform.getRight() * Wnear * 0.5f;

			// Get FarPlane size
			float Hfar = 2 * tan(m_buffer[0] / 2) * m_zfar;
			float Wfar = Hfar * m_buffer[1];
			Vector3 FarPlaneCenter = Position + Forward * m_zfar;
			Vector3 FarPlaneHalfUp = m_transform.getUp() * Hfar * 0.5f;
			Vector3 FarPlaneHalfRight = m_transform.getRight() * Wfar * 0.5f;

			// Draw ClosePlane
			Debug.DrawSquare(ClosePlaneCenter, ClosePlaneHalfUp, ClosePlaneHalfRight, Width, Color);

			// Draw FarPlane
			Debug.DrawSquare(FarPlaneCenter, FarPlaneHalfUp, FarPlaneHalfRight, Width, Color);

			// Connect Planes
			Debug.DrawLine(
				ClosePlaneCenter - ClosePlaneHalfUp - ClosePlaneHalfRight,
				FarPlaneCenter - FarPlaneHalfUp - FarPlaneHalfRight,
				Width, Color, Color
			);

			Debug.DrawLine(
				ClosePlaneCenter - ClosePlaneHalfUp + ClosePlaneHalfRight,
				FarPlaneCenter - FarPlaneHalfUp + FarPlaneHalfRight,
				Width, Color, Color
			);

			Debug.DrawLine(
				ClosePlaneCenter + ClosePlaneHalfUp - ClosePlaneHalfRight,
				FarPlaneCenter + FarPlaneHalfUp - FarPlaneHalfRight,
				Width, Color, Color
			);

			Debug.DrawLine(
				ClosePlaneCenter + ClosePlaneHalfUp + ClosePlaneHalfRight,
				FarPlaneCenter + FarPlaneHalfUp + FarPlaneHalfRight,
				Width, Color, Color
			);
		}
	}

	// Draw
	void CameraObject::Draw()
	{

	}

	void CameraObject::TransformChanged()
	{
		m_view = Matrix4x4::LookAt(m_transform.m_position, m_transform.getForward(), m_transform.getRight(), m_transform.getUp());
		m_viewInverse = m_view.Inverse();

		UpdateViewProjection();
	}
}