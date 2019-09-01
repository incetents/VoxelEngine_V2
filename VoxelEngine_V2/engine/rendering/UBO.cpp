// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "UBO.h"

#include "Graphics.h"

#include "../math/Vector.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../objects/CameraObject.h"
#include "../rendering/FramebufferObject.h"

#include "../utilities/Time.h"

namespace Vxl
{
	// UBOManager //
	void UBOManager::BindCamera(const CameraObject& _camera)
	{
		// Transpose Matrices so that they are Column Major
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera.getViewProjection().Transpose(), 0);
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera.getView().Transpose(), 64);
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera.getProjection().Transpose(), 128);
		m_ubos[UBOID::CAMERA]->Bind();
	}
	void UBOManager::BindTime()
	{
		float _time[4] = {Time.GetTime() / 20.0, Time.GetTime(), Time.GetTime() * 2.0, Time.GetTime() * 3.0};
		m_ubos[UBOID::TIME]->sendVector(Vector4(_time[0], _time[1], _time[2], _time[3]), 0);
		m_ubos[UBOID::TIME]->sendVector(Vector4(sinf(_time[0]), sinf(_time[1]), sinf(_time[2]), sinf(_time[3])), 16);
		m_ubos[UBOID::TIME]->sendVector(Vector4(cosf(_time[0]), cosf(_time[1]), cosf(_time[2]), cosf(_time[3])), 32);
		m_ubos[UBOID::TIME]->Bind();
	}
	void UBOManager::BindFBOSize(const FramebufferObject& _fbo)
	{
		m_ubos[UBOID::FBO_SIZE]->sendVector(Vector2(_fbo.GetWidth(), _fbo.GetHeight()), 0);
		m_ubos[UBOID::FBO_SIZE]->sendVector(Vector2(1.0f / _fbo.GetWidth(), 1.0f / _fbo.GetHeight()), 8);
		m_ubos[UBOID::FBO_SIZE]->Bind();
	}

	// UBO //
	void UniformBufferObject::load()
	{
		// Buffer space
		m_buffer = calloc(1, m_totalBytes);

		m_id = Graphics::UBO::Create(m_slot, m_totalBytes, BufferUsage::DYNAMIC_DRAW);

		// set name
		Graphics::SetGLName(ObjectType::BUFFER, m_id, "UBO_" + m_name);

		// unbind
		Graphics::UBO::Unbind();
	}
	void UniformBufferObject::unload()
	{
		// Buffer Space
		free(m_buffer);
		m_buffer = nullptr;

		Graphics::UBO::Delete(m_id);
	}

	void* UniformBufferObject::GetBufferLocation(unsigned int byteOffset) const
	{
		// Returns buffer point offset by certain amount of bytes
		VXL_ASSERT(byteOffset < m_totalBytes, "byte offset too large");
		return static_cast<char*>(m_buffer) + byteOffset;
	}

	void UniformBufferObject::Bind()
	{
		Graphics::UBO::Bind(m_id);

		if (m_bufferDirty)
		{
			Graphics::UBO::UpdateBuffer(m_buffer, m_totalBytes, 0);
			m_bufferDirty = false;
		}
	}
	void UniformBufferObject::Unbind() const
	{
		Graphics::UBO::Unbind();
	}

	void UniformBufferObject::sendMatrix(const Matrix2x2 &matrix, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec4]
		VXL_ASSERT(byteOffset % 16 == 0, "byteOffset not multiple of 16");

		Vector4* Destination = reinterpret_cast<Vector4*>(GetBufferLocation(byteOffset));
		Destination[0].x = matrix[0];
		Destination[0].y = matrix[1];

		Destination[1].x = matrix[2];
		Destination[1].y = matrix[3];

		m_bufferDirty = true;
	}

	void UniformBufferObject::sendMatrix(const Matrix3x3 &matrix, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec4]
		VXL_ASSERT(byteOffset % 16 == 0, "byteOffset not multiple of 16");

		Vector4* Destination = reinterpret_cast<Vector4*>(GetBufferLocation(byteOffset));
		Destination[0].x = matrix[0];
		Destination[0].y = matrix[1];
		Destination[0].z = matrix[2];

		Destination[1].x = matrix[3];
		Destination[1].y = matrix[4];
		Destination[1].z = matrix[5];

		Destination[2].x = matrix[6];
		Destination[2].y = matrix[7];
		Destination[2].z = matrix[8];

		m_bufferDirty = true;
	}

	void UniformBufferObject::sendMatrix(const Matrix4x4 &matrix, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec4]
		VXL_ASSERT(byteOffset % 16 == 0, "byteOffset not multiple of 16");
		
		Matrix4x4* Destination = reinterpret_cast<Matrix4x4*>(GetBufferLocation(byteOffset));
		*Destination = matrix;

		m_bufferDirty = true;
	}

	void UniformBufferObject::sendVector(const Vector2 &vector, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec2]
		VXL_ASSERT(byteOffset % 8 == 0, "byteOffset not multiple of 8");

		float* Destination = reinterpret_cast<float*>(GetBufferLocation(byteOffset));
		Destination[0] = vector.x;
		Destination[1] = vector.y;

		m_bufferDirty = true;
	}

	void UniformBufferObject::sendVector(const Vector3 &vector, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec4]
		VXL_ASSERT(byteOffset % 16 == 0, "byteOffset not multiple of 16");

		float* Destination = reinterpret_cast<float*>(GetBufferLocation(byteOffset));
		Destination[0] = vector.x;
		Destination[1] = vector.y;
		Destination[2] = vector.z;

		m_bufferDirty = true;
	}

	void UniformBufferObject::sendVector(const Vector4 &vector, unsigned int byteOffset)
	{
		// Must be multiple of [4 bytes] and multiple of [Vec4]
		VXL_ASSERT(byteOffset % 16 == 0, "byteOffset not multiple of 16");

		float* Destination = reinterpret_cast<float*>(GetBufferLocation(byteOffset));
		Destination[0] = vector.x;
		Destination[1] = vector.y;
		Destination[2] = vector.z;
		Destination[3] = vector.w;

		m_bufferDirty = true;
	}

}