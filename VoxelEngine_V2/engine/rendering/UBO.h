// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <map>

#include "Graphics.h"
#include "../utilities/singleton.h"
#include "../math/Vector.h"

#include <assert.h>

namespace Vxl
{
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;
	class Camera;
	class FramebufferObject;

	class UniformBufferObject
	{
		friend class UBOManager;
	private:
		uint32_t			m_id;
		const unsigned int	m_totalBytes;
		const uint32_t		m_slot;
		const std::string	m_name;
		void*				m_buffer = nullptr;
		bool				m_bufferDirty = false;

		void load();
		void unload();

		UniformBufferObject(unsigned int Bytes, uint32_t slot, const std::string& glName)
			: m_totalBytes(Bytes), m_slot(slot), m_name(glName)
		{
			load();
		}
		~UniformBufferObject()
		{
			unload();
		}

		void bind();
		void Unbind() const;

		void* GetBufferLocation(unsigned int byteOffset) const;

		void sendMatrix(const Matrix2x2& matrix, unsigned int byteOffset);
		void sendMatrix(const Matrix3x3& matrix, unsigned int byteOffset);
		void sendMatrix(const Matrix4x4& matrix, unsigned int byteOffset);
		void sendVector(const Vector2& vector, unsigned int byteOffset);
		void sendVector(const Vector3& vector, unsigned int byteOffset);
		void sendVector(const Vector4& vector, unsigned int byteOffset);

		template<typename Type>
		void sendData(Type data, unsigned int byteOffset)
		{
			// Must be multiple of [4 bytes]
			assert(byteOffset % 4 == 0);

			reinterpret_cast<Type*>(GetBufferLocation(byteOffset))[0] = data;
		
			m_bufferDirty = true;
		}
	};

	static class UBOManager : public Singleton<class UBOManager>
	{
	private:
		enum UBOID
		{
			CAMERA = 0,
			TIME = 1,
			FBO_DATA = 2,
			TOTAL
		};

		UniformBufferObject** m_ubos = nullptr;

	public:
		void InitGLResources()
		{
			// 16 = vec4
			// 64 = mat4
			m_ubos = new UniformBufferObject*[(int)UBOID::TOTAL];
			m_ubos[0] = new UniformBufferObject(64 * 3, UBOID::CAMERA, "Camera");
			m_ubos[1] = new UniformBufferObject(16 * 3, UBOID::TIME, "Time");
			m_ubos[2] = new UniformBufferObject(16 * 2, UBOID::FBO_DATA, "FBO_Data");
		}
		void DestroyGLResources()
		{
			if(m_ubos != nullptr)
				delete[] m_ubos;
		}

		void BindCamera(const Camera& _camera);
		void BindTime();
		void BindFBOSize(const FramebufferObject& _fbo);
	
	} SingletonInstance(UBOManager);
}