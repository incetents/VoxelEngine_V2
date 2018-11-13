// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

/*
#include "Rotation.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
*/
#include "Transform.h"

#include <assert.h>

namespace Vxl
{
	class CameraProjection
	{
	protected:
		// Projection Matrices
		Matrix4x4 m_projection;
		Matrix4x4 m_projectionInverse;
	public:
		CameraProjection(float znear, float zfar)
			: m_Znear(znear), m_Zfar(zfar) {}
		virtual ~CameraProjection() {}

		Matrix4x4 & getProjection()
		{
			return m_projection;
		}
		Matrix4x4 & getProjectionInverse()
		{
			return m_projection;
		}
		// Data
		float m_Znear;
		float m_Zfar;
		// Orthographic
		virtual float getXmin() const { return 0.0f; }
		virtual float getXmax() const { return 0.0f; }
		virtual float getYmin() const { return 0.0f; }
		virtual float getYmax() const { return 0.0f; }
		// Perspective
		virtual float getFov() const { return 0.0f; }
		virtual float getAspect() const { return 0.0f; }
	};

	class CameraProjection_Perspective : public CameraProjection
	{
	private:
		float m_fov;
		float m_aspect;
	public:
		float getFov() const override { return m_fov; }
		float getAspect() const override { return m_aspect; }

		void Set(float fov, float aspect)
		{
			m_fov = fov;
			m_aspect = aspect;

			m_projection.Perspective(fov, aspect, m_Znear, m_Zfar);
			m_projectionInverse = m_projection.Inverse();
		}
		CameraProjection_Perspective(float fov, float aspect, float znear, float zfar)
			: CameraProjection(znear, zfar)
		{
			Set(fov, aspect);
		}
		~CameraProjection_Perspective() {}
	};

	class CameraProjection_Orthographic : public CameraProjection
	{
	private:
		float m_boundaries[4];
	public:
		float getXmin() const override { return m_boundaries[0]; }
		float getXmax() const override { return m_boundaries[1]; }
		float getYmin() const override { return m_boundaries[2]; }
		float getYmax() const override { return m_boundaries[3]; }

		void Set(float xmin, float xmax, float ymin, float ymax)
		{
			m_boundaries[0] = xmin;
			m_boundaries[1] = xmax;
			m_boundaries[2] = ymin;
			m_boundaries[3] = ymax;

			m_projection.Orthographic(xmin, xmax, ymin, ymax, m_Znear, m_Zfar);
			m_projectionInverse = m_projection.Inverse();
		}
		CameraProjection_Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
			: CameraProjection(znear, zfar)
		{
			Set(xmin, xmax, ymin, ymax);
		}
		~CameraProjection_Orthographic() {}
	};

	class Camera
	{
	private:
		enum Type
		{
			NONE,
			ORTHOGRAPHIC,
			PERSPECTIVE
		};

		// VP DATA
		Type				m_type = Type::NONE;
		CameraProjection*	m_projection;
		Matrix4x4			m_view;
		Matrix4x4			m_viewInverse;
		Matrix4x4			m_viewProjection;
		Matrix4x4			m_viewProjectionInverse;
		
	public:
		explicit Camera(const Vector3& _position, const Vector3& _forward = Vector3(0, 0, 1), float _znear = -1.0f, float _zfar = 1.0f);
		~Camera()
		{
			delete m_projection;
		}
	
		// Data
		Transform m_transform;
		void update();

		// Utility
		Camera& setPerspective(float _fov, float _aspect);
		Camera& setOrthographic(float _xmin, float _xmax, float _ymin, float _ymax);
		inline void setZnear(float _znear)
		{
			m_projection->m_Znear = _znear;
		}
		inline void setZfar(float _zfar)
		{
			m_projection->m_Zfar = _zfar;
		}

		// Get Data	
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
			return m_projection->getProjection();
		}
		inline const Matrix4x4&	getProjectionInverse() const
		{
			return m_projection->getProjectionInverse();
		}
		inline const Matrix4x4&	getViewProjection() const
		{
			return m_viewProjection;
		}
		inline const Matrix4x4&	getViewProjectionInverse() const
		{
			return m_viewProjectionInverse;
		}

		inline Type			getType() const
		{
			return m_type;
		}
		inline float		getZnear() const
		{
			return m_projection->m_Znear;
		}
		inline float		getZfar() const
		{
			return m_projection->m_Zfar;
		}

	};
}