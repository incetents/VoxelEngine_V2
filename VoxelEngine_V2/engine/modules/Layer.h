// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Layer
	{
		friend class RenderManager;
	private:
		Layer() {} // New ones cannot be created outside rendermanager
		uint32_t m_id; // Unique bit ID
	public:
		inline const uint32_t getID(void) const
		{
			return m_id;
		}
	};
}
