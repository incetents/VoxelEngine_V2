// Copyright(c) 2018 Emmanuel Lajeunesse
#pragma once

#include "Transform.h"

#include "../utilities/Database.h"
#include <assert.h>

namespace Vxl
{
	class Transform;

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
			return m_projectionInverse;
		}
		// Data
		float m_Znear;
		float m_Zfar;
		// Orthographic
		virtual float getXmin() const { assert(false); return 0.0f; }
		virtual float getXmax() const { assert(false); return 0.0f; }
		virtual float getYmin() const { assert(false); return 0.0f; }
		virtual float getYmax() const { assert(false); return 0.0f; }
		virtual void Set(float xmin, float xmax, float ymin, float ymax) { assert(false); }
		// Perspective
		virtual float getFov() const { assert(false); return 0.0f; }
		virtual float getAspect() const { assert(false); return 0.0f; }
		virtual void Set(float _fov, float _aspect) { assert(false); }

		// Special OVerrides
		virtual void Update_X(float _xmin, float _xmax) { assert(false); }
		virtual void Update_Y(float _ymin, float _ymax) { assert(false); }
		virtual void Update_Z() { assert(false); }
		virtual void Update_FovAspect(float _fov, float _aspect) { assert(false); }

	};

	class CameraProjection_Perspective : public CameraProjection
	{
	protected:
		float m_fov;
		float m_aspect;
	public:
		float getFov() const override { return m_fov; }
		float getAspect() const override { return m_aspect; }

		void Set(float fov, float aspect) override
		{
			m_fov = fov;
			m_aspect = aspect;

			m_projection = Matrix4x4::Perspective(fov, aspect, m_Znear, m_Zfar);
			m_projectionInverse = Matrix4x4::PerspectiveInverse(fov, aspect, m_Znear, m_Zfar);
		}
		void Update_FovAspect(float _fov, float _aspect) override
		{
			Matrix4x4::Perspective_UpdateFov(m_projection, _fov, _aspect);
			Matrix4x4::PerspectiveInverse_UpdateFov(m_projectionInverse, _fov, _aspect);
		}
		void Update_Z() override
		{
			Matrix4x4::Perspective_UpdateZ(m_projection, m_Znear, m_Zfar);
			Matrix4x4::PerspectiveInverse_UpdateZ(m_projectionInverse, m_Znear, m_Zfar);
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
	protected:
		float m_boundaries[4];
	public:
		float getXmin() const override { return m_boundaries[0]; }
		float getXmax() const override { return m_boundaries[1]; }
		float getYmin() const override { return m_boundaries[2]; }
		float getYmax() const override { return m_boundaries[3]; }

		void Set(float xmin, float xmax, float ymin, float ymax) override
		{
			m_boundaries[0] = xmin;
			m_boundaries[1] = xmax;
			m_boundaries[2] = ymin;
			m_boundaries[3] = ymax;

			m_projection = Matrix4x4::Orthographic(xmin, xmax, ymin, ymax, m_Znear, m_Zfar);
			m_projectionInverse = Matrix4x4::OrthographicInverse(xmin, xmax, ymin, ymax, m_Znear, m_Zfar);
		}
		void Update_X(float _xmin, float _xmax) override
		{
			m_boundaries[0] = _xmin;
			m_boundaries[1] = _xmax;

			Matrix4x4::Orthographic_UpdateX(m_projection, _xmin, _xmax);
			Matrix4x4::OrthographicInverse_UpdateX(m_projection, _xmin, _xmax);
		}
		void Update_Y(float _ymin, float _ymax) override
		{
			m_boundaries[2] = _ymin;
			m_boundaries[3] = _ymax;

			Matrix4x4::Orthographic_UpdateY(m_projection, _ymin, _ymax);
			Matrix4x4::OrthographicInverse_UpdateY(m_projection, _ymin, _ymax);
		}
		void Update_Z() override
		{
			Matrix4x4::Orthographic_UpdateZ(m_projection, m_Znear, m_Zfar);
			Matrix4x4::OrthographicInverse_UpdateZ(m_projection, m_Znear, m_Zfar);
		}
		CameraProjection_Orthographic(float xmin, float xmax, float ymin, float ymax, float znear, float zfar)
			: CameraProjection(znear, zfar)
		{
			Set(xmin, xmax, ymin, ymax);
		}
		~CameraProjection_Orthographic() {}
	};

	class Camera : public Transform
	{
		friend class CameraProjection;
		friend class CameraProjection_Perspective;
		friend class CameraProjection_Orthographic;
		friend class RenderManager;
		friend class Window;
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
		
		static Camera* m_main;

		// Protected
		Camera(const Vector3& _position, const Vector3& _forward = Vector3(0, 0, 1), float _znear = -1.0f, float _zfar = 1.0f);
	
		// Database
		static Database<Camera> m_database;
	public:
		~Camera()
		{
			delete m_projection;
		}

		// Database Creation
		static Camera* Create(const std::string _name, const Vector3& _position, const Vector3& _forward = Vector3(0, 0, 1), float _znear = -1.0f, float _zfar = 1.0f);
	
		void update();

		static Camera* GetMain(void)
		{
			return m_main;
		}
		void SetMain(void)
		{
			m_main = this;
		}

		// Utility
		Camera& setPerspective(float _fov, float _aspect);
		Camera& setOrthographic(float _xmin, float _xmax, float _ymin, float _ymax);
		Camera& updatePerspective(float _fov, float _aspect);  // If already perspective, this will update its values faster
		Camera& updateOrtho_X(float _xmin, float _xmax);  // If already ortho, this will update its values faster
		Camera& updateOrtho_Y(float _ymin, float _ymax);  // If already ortho, this will update its values faster
		Camera& setZnear(float _znear);
		Camera& setZfar(float _zfar);

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

		inline Type	 getType() const
		{
			return m_type;
		}
		inline float getZnear() const
		{
			return m_projection->m_Znear;
		}
		inline float getZfar() const
		{
			return m_projection->m_Zfar;
		}
		inline float getFOV() const
		{
			return m_projection->getFov();
		}
		inline float getAspect() const
		{
			return m_projection->getAspect();
		}
		inline float getXmin() const
		{
			return m_projection->getXmin();
		}
		inline float getXmax() const
		{
			return m_projection->getXmax();
		}
		inline float getYmin() const
		{
			return m_projection->getYmin();
		}
		inline float getYmax() const
		{
			return m_projection->getYmax();
		}

		// Override for transform class (forward directions are reversed)
		Vector3 getForward(void) override
		{
			updateValues();
			return -m_forward;
		}
		Vector3 getBackwards(void) override
		{
			updateValues();
			return m_forward;
		}

		Transform& setForward(const Vector3& forward);
	};
}