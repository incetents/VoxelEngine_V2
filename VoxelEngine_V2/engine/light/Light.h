// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace Vxl
{
	class Light
	{
	private:
		Vector3 m_position;

	public:
		Light(Vector3 position)
			: m_position(position)
		{}

		inline void SetPosition(Vector3 position)
		{
			m_position = position;
		}
		inline Vector3 GetPosition(void) const
		{
			return m_position;
		}

	};
}