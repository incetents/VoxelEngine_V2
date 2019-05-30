// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "UBO.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix2x2.h"
#include "../math/Matrix3x3.h"
#include "../math/Matrix4x4.h"
#include "../objects/CameraObject.h"

#include <assert.h>

namespace Vxl
{
	// UBOManager //
	void UBOManager::BindCamera(CameraObject* _camera)
	{
		// Transpose Matrices so that they are Column Major
		m_ubos[UBOID::CAMERA]->Bind();
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera->getViewProjection().Transpose(), 0);
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera->getView().Transpose(), 64);
		m_ubos[UBOID::CAMERA]->sendMatrix(_camera->getProjection().Transpose(), 128);
	}

	// UBO //
	void UniformBufferObject::load()
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
		
		// Allocate size
		glBufferData(GL_UNIFORM_BUFFER, m_totalBytes, NULL, GL_DYNAMIC_DRAW);

		// Set to slot
		glBindBufferBase(GL_UNIFORM_BUFFER, m_slot, m_id);

		// set name
		glUtil::setGLName(glNameType::BUFFER, m_id, "UBO_" + m_name);

		// unbind
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void UniformBufferObject::unload()
	{
		glDeleteBuffers(1, &m_id);
	}

	void UniformBufferObject::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	}
	void UniformBufferObject::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBufferObject::sendMatrix(const Matrix2x2 &matrix, int offset) const
	{
		assert(offset % 16 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Matrix2x2), &matrix);
		Unbind();
	}

	void UniformBufferObject::sendMatrix(const Matrix3x3 &matrix, int offset) const
	{
		assert(offset % 16 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Matrix3x3), &matrix);
		Unbind();
	}

	void UniformBufferObject::sendMatrix(const Matrix4x4 &matrix, int offset) const
	{
		assert(offset % 16 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Matrix4x4), &matrix);
		Unbind();
	}

	void UniformBufferObject::sendVector(const Vector2 &vector, int offset) const
	{
		assert(offset % 8 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Vector2), &vector);
		Unbind();
	}

	void UniformBufferObject::sendVector(const Vector3 &vector, int offset) const
	{
		assert(offset % 16 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Vector3), &vector);
		Unbind();
	}

	void UniformBufferObject::sendVector(const Vector4 &vector, int offset) const
	{
		assert(offset % 16 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Vector4), &vector);
		Unbind();
	}

	void UniformBufferObject::sendFloat(float scalar, int offset) const
	{
		assert(offset % 4 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &scalar);
		Unbind();
	}

	void UniformBufferObject::sendInt(int number, int offset) const
	{
		assert(offset % 4 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &number);
		Unbind();
	}

	void UniformBufferObject::sendUInt(unsigned int number, int offset) const
	{
		assert(offset % 4 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(unsigned int), &number);
		Unbind();
	}

	void UniformBufferObject::sendBool(bool boolean, int offset) const
	{
		assert(offset % 4 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(bool), &boolean);
		Unbind();
	}

	void UniformBufferObject::sendData(void * data, int size, int offset) const
	{
		assert(offset % 4 == 0);
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		Unbind();
	}

}