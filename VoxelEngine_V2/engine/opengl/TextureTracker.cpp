//// Copyright (c) 2019 Emmanuel Lajeunesse
//#include "Precompiled.h"
//#include "TextureTracker.h"
//
//#include "Texture.h"
//
//namespace Vxl
//{
//	bool TextureTracker::ShouldBindTexture(Active_Texture Active, UINT ID)
//	{
//		// New Texture
//		if (m_activeTextures.find(Active) == m_activeTextures.end())
//			m_activeTextures[Active] = ID;
//		// Existing Texture
//		else if (m_activeTextures[Active] == ID)
//			return false;
//		// Should Bind
//		return true;
//	}
//	void TextureTracker::SetCurrentTexture(Active_Texture Active, UINT ID)
//	{
//		m_activeTextures[Active] = ID;
//	}
//
//	void TextureTracker::BindSafe(Active_Texture Active, BaseTexture* tex)
//	{
//		if (ShouldBindTexture(Active, tex->GetID()))
//		{
//			SetCurrentTexture(Active, tex->GetID());
//			tex->Bind(Active);
//		}
//	}
//
//	void TextureTracker::Clear()
//	{
//		m_activeTextures.clear();
//	}
//}
