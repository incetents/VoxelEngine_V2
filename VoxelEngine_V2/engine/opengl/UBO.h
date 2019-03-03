// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "glUtil.h"


namespace Vxl
{
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;

	class UniformBufferObject
	{
	private:
		GLuint m_id;

		void load(unsigned int Bytes, GLuint slot, const std::string& glName);
		void unload();

	public:
		UniformBufferObject(unsigned int Bytes, GLuint slot, const std::string& glName)
		{
			load(Bytes, slot, glName);
		}
		~UniformBufferObject()
		{
			unload();
		}

		void Bind() const;
		void Unbind() const;

		void sendMatrix(const Matrix2x2& matrix, int offset) const;
		void sendMatrix(const Matrix3x3& matrix, int offset) const;
		void sendMatrix(const Matrix4x4& matrix, int offset) const;
		void sendVector(const Vector2& vector, int offset) const;
		void sendVector(const Vector3& vector, int offset) const;
		void sendVector(const Vector4& vector, int offset) const;
		void sendFloat(float scalar, int offset) const;
		void sendInt(int scalar, int offset) const;
		void sendUInt(unsigned int scalar, int offset) const;
		void sendBool(bool boolean, int offset) const;
		void sendData(void* data, int size, int offset = 0) const;

	};

}