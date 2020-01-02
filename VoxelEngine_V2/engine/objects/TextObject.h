//// Copyright(c) 2020 Emmanuel Lajeunesse
//#pragma once
//
//#include "../objects/GameObject.h"
//#include "../utilities/singleton.h"
//
//namespace Vxl
//{
//	// Forward Declaration
//	class Mesh;
//	class RenderTexture;
//	class Texture2D;
//	class FramebufferObject;
//	class VAO;
//	template<typename Type>
//	class MeshBufferMem;
//
//	struct RenderCharacter
//	{
//		Texture2D* m_texture;
//		Vector2i m_size;
//		Vector2i m_bearing;
//		uint32_t m_advance;
//		float m_yOffset;
//		float m_advanceFlt;
//		float m_bboxWidth;
//		float m_bboxHeight;
//	};
//
//	class RenderFont
//	{
//	public:
//		static std::map<std::string, RenderFont*> m_fonts;
//
//		std::map<char, RenderCharacter> m_characters;
//		uint32_t	m_fontHeight;
//		float		m_largestYOffset = 0;
//
//		RenderFont(const std::string& filepath, uint32_t fontSize);
//	};
//
//	static class GlobalRenderText : public Singleton<class GlobalRenderText>
//	{
//	public:
//		// Graphics Resources [ Shared ]
//		static FramebufferObject*	m_FBO;
//		static VAO*					m_VAO;
//		static MeshBufferMem<float>*m_buffer;
//
//		// Global Resources
//		static void InitGLResources();
//		static void DestoryGLResources();
//
//	} SingletonInstance(GlobalRenderText);
//
//	class RenderText
//	{
//		friend class TextObject;
//	private:
//		// Font info
//		RenderFont*		m_font = nullptr;
//		bool			m_fontDirty = false;
//
//		// Render Texture
//		RenderTexture*	m_renderTexture = nullptr;
//		bool			m_renderTextureDirty = false;
//		Vector2ui		m_renderTextureTargetSize;
//
//		// Text
//		std::string m_text;
//		uint64_t	m_lineCount = 0;
//
//		// Update internal data for text render
//		void Update(const std::string& text);
//		void UpdateRenderTexture();
//
//	};
//
//	class TextObject : public GameObject
//	{
//		friend class RenderManager;
//	private:
//		TextObject(const std::string& name);
//
//		// Data
//		RenderText renderText;
//
//	public:
//		// Destructor
//		~TextObject();
//
//		// Asset Creation
//		static TextObject* Create(const std::string& name);
//
//		// Utility
//		static void LoadFont(const std::string& font, const std::string& path);
//		bool SetFont(const std::string& font);
//		void SetText(const std::string& text);
//
//		// Locked out functions
//		void SetMesh(Mesh* mesh) override
//		{
//			VXL_ASSERT(false, "TextObject SetMesh should only be called from Constructor");
//		}
//		void SetMaterial(Material* material) override
//		{
//			VXL_ASSERT(false, "TextObject SetMaterial should only be called from Constructor");
//		}
//
//		// Behaviour
//		virtual void Update() override;
//		virtual void Draw() override;
//	private:
//		virtual void TransformChanged() override;
//	};
//}