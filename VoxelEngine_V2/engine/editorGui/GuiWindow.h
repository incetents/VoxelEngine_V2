// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/Macros.h"
#include "../math/Vector.h"

namespace Vxl
{
	class Scene;

	class GuiWindow
	{
	protected:
		std::string m_name;
		Scene*	m_scene = nullptr;
		Vector2 m_size;
		float	m_alpha;
		int		m_flags = 0;
		bool	m_open = true;
		bool	m_padding = true;
		
	public:
		inline void Init(std::string name, Vector2 initSize, float alpha, int flags = 0)
		{
			m_size = initSize;
			m_name = name;
			m_alpha = alpha;
			m_flags = flags;
		}

		// Utility
		void		SetOpen(bool state)
		{
			m_open = state;
		}
		void		ToggleOpen(void)
		{
			m_open = !m_open;
		}
		void		SetScene(Scene* scene)
		{
			m_scene = scene;
		}
		bool		IsOpen(void) const
		{
			return m_open;
		}
		std::string GetName(void) const
		{
			return m_name;
		}
		void		SetPadding(bool state)
		{
			m_padding = state;
		}

		// Behaviour
		bool Begin();
		void End();
		virtual void Draw() = 0;
	};
}