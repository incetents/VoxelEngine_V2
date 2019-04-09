// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <map>

#include "glUtil.h"
#include "../utilities/singleton.h"

namespace Vxl
{
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;
	class CameraObject;

	class UniformBufferObject
	{
		friend class UBOManager;
	private:
		GLuint m_id;
		const unsigned int m_totalBytes;
		const GLuint m_slot;
		const std::string m_name;

		void load();
		void unload();

		UniformBufferObject(unsigned int Bytes, GLuint slot, const std::string& glName)
			: m_totalBytes(Bytes), m_slot(slot), m_name(glName)
		{
			load();
		}
		~UniformBufferObject()
		{
			unload();
		}
	public:

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

	static class UBOManager : public Singleton<class UBOManager>
	{
	private:
		enum UBOID
		{
			CAMERA = 0,
			FAKE
		};

		UniformBufferObject** m_ubos;

	public:
		void Setup()
		{
			delete[] m_ubos;
			m_ubos = new UniformBufferObject*[2];
			m_ubos[0] = new UniformBufferObject(64 * 3, UBOID::CAMERA, "Camera");
			m_ubos[1] = new UniformBufferObject(0, UBOID::FAKE, "Empty");
		}
		void BindCamera(CameraObject* _camera);

		~UBOManager()
		{
			delete[] m_ubos;
		}
	
	} SingletonInstance(UBOManager);
}