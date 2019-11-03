// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "TextureBinder.h"

#include "../rendering/Debug.h"
#include "../modules/GlobalData.h"

#include "Texture2D.h"

namespace Vxl
{
	void TextureBinder::BindTextures()
	{
		// Null Texture if no textures are used
		if (m_textures.empty())
		{
			// Bind null texture on first active layer
			GlobalData.tex_nullImage->Bind(TextureLevel::LEVEL0);
		}
		else
		{
			for (auto it = m_textures.begin(); it != m_textures.end(); it++)
			{
				// Get Current texture
				BaseTexture* _tex = m_textures[it->first];

				// Bind Null texture if texture isn't loaded
				if (_tex == nullptr || !_tex->IsLoaded())
				{
					GlobalData.tex_nullImage->Bind(TextureLevel::LEVEL0);
				}
				// Bind texture normally
				else
				{
					_tex->Bind(it->first);
				}
			}
		}
	}
}