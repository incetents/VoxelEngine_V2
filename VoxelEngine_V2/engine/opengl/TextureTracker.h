//// Copyright (c) 2019 Emmanuel Lajeunesse
//#pragma once
//
//#include "../utilities/singleton.h"
//#include "../opengl/glUtil.h"
//#include <unordered_map>
//
//namespace Vxl
//{
//	class BaseTexture;
//
//	static class TextureTracker : public Singleton<class TextureTracker>
//	{
//	private:
//		std::unordered_map<Active_Texture, UINT> m_activeTextures;
//		bool ShouldBindTexture(Active_Texture Active, UINT ID);
//		void SetCurrentTexture(Active_Texture Active, UINT ID);
//	public:
//		// Doesn't bind texture if already bound at the same slot
//		void BindSafe(Active_Texture Active, BaseTexture* tex);
//		void Clear();
//
//	} SingletonInstance(TextureTracker);
//}
//
