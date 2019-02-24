// Copyright (c) 2018 Emmanuel Lajeunesse
#pragma once

namespace Vxl
{
	class Layer
	{
		friend class RenderManager;
	private:
		Layer() {} // New ones cannot be created outside rendermanager
		UINT m_id; // Unique bit ID
	public:
		inline const UINT GetID(void) const
		{
			return m_id;
		}
	};
}
