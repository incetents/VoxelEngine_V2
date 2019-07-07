// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"
#include "../rendering/UBO.h"

namespace Vxl
{
	class UniformBufferObject;

	enum CameraType
	{
		NONE,
		ORTHOGRAPHIC,
		PERSPECTIVE,
		PERSPECTIVE_WINDOW_ASPECT,
		CUSTOM
	};

	class CameraObject : public Entity, public Asset<CameraObject>
	{
		friend class RenderManager;
	private:
		CameraObject(const std::string& name, float _znear, float _zfar);

		// Data
		CameraType		m_type = CameraType::NONE;
		float			m_znear;
		float			m_zfar;
		float			m_buffer[4] = { 0,0,0,0 }; // Stores [fov,aspect,0,0] or [xmin,xmax,ymin,ymax]
		
		// Matrix Data
		Matrix4x4 m_projection;
		Matrix4x4 m_projectionInverse;
		Matrix4x4 m_view;
		Matrix4x4 m_viewInverse;
		Matrix4x4 m_viewProjection;
		Matrix4x4 m_viewProjectionInverse;

		inline void UpdateViewProjection()
		{
			m_viewProjection = m_projection * m_view;
			m_viewProjectionInverse = m_viewProjection.Inverse();
		}

		// Projection Matrix Creator (based on stored data)
		void CreatePerspective();
		// Projection Matrix Creator (based on stored data)
		void CreateOrthographic();

		// Data updates [Doesn't affect Projection Matrix]
		void UpdateFOV(float _fov);
		void UpdateAspect(float _aspect);
		void UpdateOrthoX(float _xmin, float _xmax);
		void UpdateOrthoY(float _ymin, float _ymax);

	public:

		//void BindUBO();

		// Destructor
		~CameraObject();

		// Database Creation
		static CameraObject* Create(const std::string& _name, float _znear, float _zfar);

		// Zplane
		void SetZNear(float _znear)
		{
			SetZNearAndFar(_znear, m_zfar);
		}
		void SetZFar(float _zfar)
		{
			SetZNearAndFar(m_znear, _zfar);
		}
		void SetZNearAndFar(float _znear, float _zfar)
		{
			m_znear = _znear;
			m_zfar = _zfar;
			switch (m_type)
			{
			case CameraType::PERSPECTIVE_WINDOW_ASPECT:
			case CameraType::PERSPECTIVE:
				CreatePerspective();
				break;
			case CameraType::ORTHOGRAPHIC:
				CreateOrthographic();
				break;
			}
		}
		inline float GetZNear(void) const
		{
			return m_znear;
		}
		inline float GetZFar(void) const
		{
			return m_zfar;
		}

		// Set Type
		void SetPerspectiveWindowAspect(float _fov);
		void SetPerspective(float _fov, float _aspect);
		void SetOrthographic(float _xmin, float _xmax, float _ymin, float _ymax);
		void SetCustom(const Matrix4x4& _projection);

		// Checkers
		inline bool isPerspectiveWindowAspect(void) const
		{
			return m_type == CameraType::PERSPECTIVE_WINDOW_ASPECT;
		}
		inline bool isPerspective(void) const
		{
			return m_type == CameraType::PERSPECTIVE;
		}
		inline bool isOrthographic(void) const
		{
			return m_type == CameraType::ORTHOGRAPHIC;
		}
		inline bool isCustom(void) const
		{
			return m_type == CameraType::CUSTOM;
		}

		// Get Data
		inline float getZnear() const
		{
			return m_znear;
		}
		inline float getZfar() const
		{
			return m_zfar;
		}
		// [ DEGREES ]
		inline float getFOV() const
		{
			return m_buffer[0];
		}
		inline float getAspect() const
		{
			return m_buffer[1];
		}
		inline float getXmin() const
		{
			return m_buffer[0];
		}
		inline float getXmax() const
		{
			return m_buffer[1];
		}
		inline float getYmin() const
		{
			return m_buffer[2];
		}
		inline float getYmax() const
		{
			return m_buffer[3];
		}
		// Get Matrix	
		inline const Matrix4x4&	getView() const
		{
			return m_view;
		}
		inline const Matrix4x4&	getViewInverse() const
		{
			return m_viewInverse;
		}
		inline const Matrix4x4&	getProjection() const
		{
			return m_projection;
		}
		inline const Matrix4x4&	getProjectionInverse() const
		{
			return m_projectionInverse;
		}
		inline const Matrix4x4&	getViewProjection() const
		{
			return m_viewProjection;
		}
		inline const Matrix4x4&	getViewProjectionInverse() const
		{
			return m_viewProjectionInverse;
		}

		// [ DEBUG ]
		void DrawFrustum(float Width, Color4F Color);

		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	public:
		virtual void TransformChanged() override;
	};
}