// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "TextureTracker.h"

namespace Vxl
{
	bool TextureTracker::ShouldBindTexture(Active_Texture Active, UINT ID)
	{
		// New Texture
		if (m_activeTextures.find(Active) == m_activeTextures.end())
			m_activeTextures[Active] = ID;
		// Existing Texture
		else if (m_activeTextures[Active] == ID)
			return false;
		// Should Bind
		return true;
	}
	void TextureTracker::SetCurrentTexture(Active_Texture Active, UINT ID)
	{
		m_activeTextures[Active] = ID;
	}
	void TextureTracker::NewFrame()
	{
		m_activeTextures.clear();
	}

}
