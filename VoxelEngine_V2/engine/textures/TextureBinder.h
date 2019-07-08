// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "BaseTexture.h"

#include <map>

namespace Vxl
{
	// Stores set of textures for binding purposes
	class TextureBinder
	{
	private:
		std::map<TextureLevel, BaseTexture*> m_textures;

	protected:
		// Binds all textures to active layers
		void BindTextures();

	public:
		inline void SetTexture(BaseTexture* texture, TextureLevel layer)
		{
			m_textures[layer] = texture;
		}
		inline void RemoveTexture(TextureLevel layer)
		{
			m_textures.erase(layer);
		}
		inline void ClearTextures(void)
		{
			m_textures.clear();
		}

		
	};
}