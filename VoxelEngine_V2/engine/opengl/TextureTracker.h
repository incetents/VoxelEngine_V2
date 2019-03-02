// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../utilities/singleton.h"
#include "../opengl/glUtil.h"
#include <unordered_map>

namespace Vxl
{
	static class TextureTracker : public Singleton<class TextureTracker>
	{
	private:
		std::unordered_map<Active_Texture, UINT> m_activeTextures;
	public:
		bool ShouldBindTexture(Active_Texture Active, UINT ID);
		void SetCurrentTexture(Active_Texture Active, UINT ID);
		void NewFrame();

	} SingletonInstance(TextureTracker);
}

