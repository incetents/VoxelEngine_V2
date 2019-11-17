// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <vector>

#include "Types.h"

#include "../math/Color.h"
#include "../utilities/Logger.h"
#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

// Used as a means of inheriting a unique map of objects //
namespace Vxl
{
	// Forward Declare Classes
	class BaseTexture;
	class Texture2D;
	class Cubemap;
	class RenderTexture;
	struct File;
	class RenderTexture;
	class RenderTextureDepth;
	class RenderBuffer;
	class RenderBufferDepth;
	class FramebufferObject;
	class Mesh;
	class Shader;
	class ShaderProgram;
	class ShaderMaterial;
	class Material;
	class Entity;
	class Camera;
	// Forward Declare Enums
	enum class TextureWrapping;
	enum class TextureFilter;
	enum class TextureFormat;
	enum class TexturePixelType;
	enum class TextureChannelType;
	enum class AnisotropicMode;
	enum class TextureDepthFormat;
	enum class ShaderType;

	enum class AssetType
	{
		GLOBAL,
		SCENE
	};

	template<class Type>
	class IDStorage
	{
		DISALLOW_COPY_AND_ASSIGN(IDStorage);
	public:
		IDStorage() {}

		static uint32_t				m_nextID;
		std::stack<uint32_t>		m_deletedIDs;
		std::map<uint32_t, Type*>	m_storage;
		std::map<uint32_t, Type*>	m_storage_typeGlobal;
		std::map<uint32_t, Type*>	m_storage_typeScene;
		
		// Used as a special case to override setting automatic ID
		uint32_t	AddCustom(Type* data, AssetType type, uint32_t customID)
		{
			m_storage[customID] = data;

			if (type == AssetType::GLOBAL)
				m_storage_typeGlobal[customID] = data;
			else
				m_storage_typeScene[customID] = data;

			return customID;
		}
		uint32_t	Add(Type* data, AssetType type)
		{
			uint32_t NewID;

			// use a spare ID
			if (m_deletedIDs.size() > 0)
			{
				NewID = m_deletedIDs.top();
				m_deletedIDs.pop();
			}
			// create a new ID
			else
				NewID = m_nextID++;

			// Store
			m_storage[NewID] = data;
			if (type == AssetType::GLOBAL)
				m_storage_typeGlobal[NewID] = data;
			else
				m_storage_typeScene[NewID] = data;

			// Get ID
			return NewID;
		}
		Type*		Get(uint32_t id)
		{
			// return data
			auto it = m_storage.find(id);
			if (it != m_storage.end())
				return it->second;

			// not found
			return nullptr;
		}
		Type*		Erase(uint32_t id)
		{
			auto it = m_storage.find(id);
			if (it != m_storage.end())
			{
				m_deletedIDs.push(id);
				// Get data
				Type* data = m_storage[id];
				// Erase
				m_storage.erase(id);
				m_storage_typeGlobal.erase(id);
				m_storage_typeScene.erase(id);
				//
				return data;
			}
			return nullptr;
		}
		void		EraseAll(void)
		{
			m_nextID = 1;
			m_deletedIDs = std::stack<uint32_t>();

			m_storage_typeGlobal.clear();
			m_storage_typeScene.clear();
			m_storage.clear();
		}
		void		EraseAll(AssetType type)
		{
			if (type == AssetType::GLOBAL)
			{
				m_storage_typeGlobal.clear();
				m_storage = m_storage_typeScene;

				if (m_storage.empty())
				{
					m_nextID = 1;
					m_deletedIDs = std::stack<uint32_t>();
				}
				else
				{
					for (auto& data : m_storage_typeGlobal)
						m_deletedIDs.push(data.first);
				}
			}
			else
			{
				m_storage_typeScene.clear();
				m_storage = m_storage_typeGlobal;

				if (m_storage.empty())
				{
					m_nextID = 1;
					m_deletedIDs = std::stack<uint32_t>();
				}
				else
				{
					for (auto& data : m_storage_typeGlobal)
						m_deletedIDs.push(data.first);
				}
			}
		}
		std::map<uint32_t, Type*>& GetAll(void)
		{
			return m_storage;
		}
		std::map<uint32_t, Type*>& GetAll(AssetType type)
		{
			if (type == AssetType::GLOBAL)
				return m_storage_typeGlobal;
			else if (type == AssetType::SCENE)
				return m_storage_typeScene;

			return m_storage;
		}
	};

	template<class Type>
	uint32_t IDStorage<Type>::m_nextID = 1;

	template<class Type>
	class NamedStorage
	{
		DISALLOW_COPY_AND_ASSIGN(NamedStorage);
	public:
		NamedStorage() {}

		std::map<std::string, Type*> m_storage;
		std::map<std::string, Type*> m_storage_typeGlobal;
		std::map<std::string, Type*> m_storage_typeScene;

		void	Add(const std::string& name, Type* data, AssetType type)
		{
			m_storage[name] = data;

			if (type == AssetType::GLOBAL)
				m_storage_typeGlobal[name] = data;
			else
				m_storage_typeScene[name] = data;
		}
		Type*		Get(const std::string& name)
		{
			// return data
			if (m_storage.find(name) != m_storage.end())
				return m_storage[name];

			// not found
			return nullptr;
		}
		Type*		Erase(const std::string& name)
		{
			if (m_storage.find(name) != m_storage.end())
			{
				Type* data = m_storage[name];
				m_storage.erase(name);
				return data;
			}
			return nullptr;
		}
		void		EraseAll(void)
		{
			m_storage.clear();
		}
		void		EraseAll(AssetType type)
		{
			if (type == AssetType::GLOBAL)
			{
				m_storage_typeGlobal.clear();
				m_storage = m_storage_typeScene;
			}
			else
			{
				m_storage_typeScene.clear();
				m_storage = m_storage_typeGlobal;
			}
		}
		std::map<std::string, Type*>& GetAll(void)
		{
			return m_storage;
		}
		std::map<std::string, Type*>& GetAll(AssetType type)
		{
			if (type == AssetType::GLOBAL)
				return m_storage_typeGlobal;
			else if (type == AssetType::SCENE)
				return m_storage_typeScene;

			return m_storage;
		}
	};

	class Assets
	{
		DISALLOW_COPY_AND_ASSIGN(Assets);
		friend class RenderManager;
	protected:
		static IDStorage<BaseTexture>		 m_baseTexture_storage;
		static IDStorage<Texture2D>			 m_texture2D_storage;
		static IDStorage<Cubemap>			 m_cubemap_storage;
		static NamedStorage<File>			 m_file_storage;
		static IDStorage<FramebufferObject>	 m_framebufferObject_storage;
		static IDStorage<RenderTexture>		 m_renderTexture_storage;
		static IDStorage<RenderTextureDepth> m_renderTextureDepth_storage;
		static IDStorage<RenderBuffer>		 m_renderBuffer_storage;
		static IDStorage<RenderBufferDepth>	 m_renderBufferDepth_storage;
		static IDStorage<Mesh>				 m_mesh_storage;
		static IDStorage<ShaderProgram>		 m_shaderProgram_storage;
		static IDStorage<ShaderMaterial>	 m_shaderMaterial_storage;
		static IDStorage<Material>			 m_material_storage;
		static IDStorage<Entity>			 m_entity_storage;
		static IDStorage<Camera>			 m_camera_storage;

		void DestroyAndEraseAll();

		AssetType m_creationType;

		// Protected
		Assets() {}

	public:
		// Remove from Asset Management [Still needs to be deleted manually]
		static BaseTexture*			eraseBaseTexture(TextureIndex index);
		static Texture2D*			eraseTexture2D(TextureIndex index) { m_baseTexture_storage.Erase(index); return m_texture2D_storage.Erase(index); }
		static Cubemap*				eraseCubemap(TextureIndex index) { m_baseTexture_storage.Erase(index); return m_cubemap_storage.Erase(index); }
		static File*				eraseFile(const std::string& name) { return m_file_storage.Erase(name); }
		static FramebufferObject*	eraseFramebufferObject(FramebufferObjectIndex index) { return m_framebufferObject_storage.Erase(index);  }
		static RenderTexture*		eraseRenderTexture(RenderTextureIndex index) { return m_renderTexture_storage.Erase(index); }
		static RenderTextureDepth*	eraseRenderTextureDepth(RenderTextureDepthIndex index) { return m_renderTextureDepth_storage.Erase(index); }
		static RenderBuffer*		eraseRenderBuffer(RenderBufferIndex index) { return m_renderBuffer_storage.Erase(index); }
		static RenderBufferDepth*	eraseRenderBufferDepth(RenderBufferDepthIndex index) { return m_renderBufferDepth_storage.Erase(index); }
		static Mesh*				eraseMesh(MeshIndex index) { return m_mesh_storage.Erase(index); }
		static ShaderProgram*		eraseShaderProgram(ShaderProgramIndex index) { return m_shaderProgram_storage.Erase(index); }
		static ShaderMaterial*		eraseShaderMaterial(ShaderMaterialIndex index) { return m_shaderMaterial_storage.Erase(index); }
		static Material*			eraseMaterial(MaterialIndex index) { return m_material_storage.Erase(index); }
		static Entity*				eraseEntity(EntityIndex index) { return m_entity_storage.Erase(index); }
		static Camera*				eraseCamera(CameraIndex index) { return m_camera_storage.Erase(index); }

		// Delete [Remove and delete from Asset Management]
		static void deleteBaseTexture(TextureIndex index);
		static void deleteTexture2D(TextureIndex index);
		static void deleteCubemap(TextureIndex index);
		static void deleteFile(const std::string& name);
		static void deleteFramebufferObject(FramebufferObjectIndex index);
		static void deleteRenderTexture(RenderTextureIndex index);
		static void deleteRenderTextureDepth(RenderTextureDepthIndex index);
		static void deleteRenderBuffer(RenderBufferIndex index);
		static void deleteRenderBufferDepth(RenderBufferDepthIndex index);
		static void deleteMesh(MeshIndex index);
		static void deleteShaderProgram(ShaderProgramIndex index);
		static void deleteShaderMaterial(ShaderMaterialIndex index);
		static void deleteMaterial(MaterialIndex index);
		static void deleteEntity(EntityIndex index);
		static void deleteCamera(CameraIndex index);

		// Delete All [Remove and delete from Asset Management]
		static void deleteAllTextures();
		//static void deleteAllTexture2D();
		//static void deleteAllCubemap();
		static void deleteAllFile();
		static void deleteAllFramebufferObject();
		static void deleteAllRenderTexture();
		static void deleteAllRenderTextureDepth();
		static void deleteAllRenderBuffer();
		static void deleteAllRenderBufferDepth();
		static void deleteAllMesh();
		static void deleteAllShaderProgram();
		static void deleteAllShaderMaterial();
		static void deleteAllMaterial();
		static void deleteAllEntity();
		static void deleteAllCamera();

		// Getters
		static BaseTexture*			getBaseTexture(TextureIndex index) { return m_baseTexture_storage.Get(index); }
		static Texture2D*			getTexture2D(TextureIndex index) { return m_texture2D_storage.Get(index); }
		static Cubemap*				getCubemap(TextureIndex index) { return m_cubemap_storage.Get(index); }
		static File*				getFile(const std::string& name) { return m_file_storage.Get(stringUtil::toLowerCopy(name)); }
		static FramebufferObject*	getFramebufferObject(FramebufferObjectIndex index) { return m_framebufferObject_storage.Get(index); }
		static RenderTexture*		getRenderTexture(RenderTextureIndex index) { return m_renderTexture_storage.Get(index); }
		static RenderTextureDepth*	getRenderTextureDepth(RenderTextureDepthIndex index) { return m_renderTextureDepth_storage.Get(index); }
		static RenderBuffer*		getRenderBuffer(RenderBufferIndex index) { return m_renderBuffer_storage.Get(index); }
		static RenderBufferDepth*	getRenderBufferDepth(RenderBufferDepthIndex index) { return m_renderBufferDepth_storage.Get(index); }
		static Mesh*				getMesh(MeshIndex index) { return m_mesh_storage.Get(index); }
		static ShaderProgram*		getShaderProgram(ShaderProgramIndex index) { return m_shaderProgram_storage.Get(index); }
		static ShaderMaterial*		getShaderMaterial(ShaderMaterialIndex index) { return m_shaderMaterial_storage.Get(index); }
		static Material*			getMaterial(MaterialIndex index) { return m_material_storage.Get(index); }
		static Entity*				getEntity(EntityIndex index) { return m_entity_storage.Get(index); }
		static Camera*				getCamera(CameraIndex index) { return m_camera_storage.Get(index); }

		// Get All
		static const std::map<uint32_t, BaseTexture*>&			getAllBaseTexture() { return m_baseTexture_storage.GetAll(); }
		static const std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(); }
		static const std::map<uint32_t, Cubemap*>&				getAllCubemap() { return m_cubemap_storage.GetAll(); }
		static const std::map<std::string, File*>&				getAllFiles() { return m_file_storage.GetAll(); }
		static const std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(); }
		static const std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(); }
		static const std::map<uint32_t, RenderTextureDepth*>&	getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(); }
		static const std::map<uint32_t, RenderBuffer*>&			getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(); }
		static const std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(); }
		static const std::map<uint32_t, Mesh*>&					getAllMesh() { return m_mesh_storage.GetAll(); }
		static const std::map<uint32_t, ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(); }
		static const std::map<uint32_t, ShaderMaterial*>&		getAllShaderMaterial() { return m_shaderMaterial_storage.GetAll(); }
		static const std::map<uint32_t, Material*>&				getAllMaterial() { return m_material_storage.GetAll(); }
		static const std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(); }
		static const std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(); }

		//
		TextureIndex loadTexture2D(
			const std::string& filePath,
			bool				InvertY,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			TextureFormat		FormatType,
			TexturePixelType	PixelType,
			AnisotropicMode		AnisotropicMode
		);
		TextureIndex createTexture2D(
			std::vector<float> pixels, uint32_t width,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			AnisotropicMode		AnisotropicMode
		);
		TextureIndex createTexture2D(
			std::vector<Color3F> pixels, uint32_t width,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			AnisotropicMode		AnisotropicMode
		);
		TextureIndex createTexture2D(
			std::vector<Color4F> pixels, uint32_t width,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			AnisotropicMode		AnisotropicMode
		);
		TextureIndex createTexture2D(
			void* pixels, uint32_t width, uint32_t height,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			TextureFormat		FormatType,
			TextureChannelType	ChannelType,
			TexturePixelType	PixelType,
			AnisotropicMode		AnisotropicMode
		);
		//
		TextureIndex loadCubemap(
			const std::string& filePath1, const std::string& filePath2,
			const std::string& filePath3, const std::string& filePath4,
			const std::string& filePath5, const std::string& filePath6,
			bool InvertY,
			bool UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			TextureFormat		FormatType,
			TexturePixelType	PixelType,
			AnisotropicMode		AnisotropicMode
		);
		//
		void loadFile(
			const std::string& name,
			const std::string& filepath
		);
		//
		FramebufferObjectIndex createFramebuffer();
		//
		RenderTextureIndex createRenderTexture(
			int Width,
			int Height,
			TextureFormat FormatType,
			TexturePixelType PixelType,
			bool MipMapping
		);
		//
		RenderTextureDepthIndex createRenderTextureDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);
		//
		RenderBufferIndex createRenderBuffer(
			int Width,
			int Height,
			TextureFormat FormatType,
			TexturePixelType PixelType
		);
		//
		RenderBufferDepthIndex createRenderBufferDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);
		//
		MeshIndex createMesh();
		//
		ShaderProgramIndex createShaderProgram(const std::string& name, const std::vector<Shader*>& _shaders);
		ShaderProgramIndex createShaderProgram(const std::string& name, std::initializer_list<Shader*> _shaders);
		//
		MaterialIndex createMaterial(const std::string& name);
		//
		ShaderMaterialIndex createShaderMaterial(const std::string& filePath);
		//
		EntityIndex createEntity(const std::string& name);
		//
		CameraIndex createCamera(const std::string& name, float znear = 0.1f, float zfar = 100.0f);
		
	
	};
	
	static class SceneAssets : public Assets, public Singleton<class SceneAssets>
	{
		DISALLOW_COPY_AND_ASSIGN(SceneAssets);
	public:
		SceneAssets()
		{
			m_creationType = AssetType::SCENE;
		}

		// Get All
		const std::map<uint32_t, BaseTexture*>&			getAllBaseTexture() { return m_baseTexture_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Cubemap*>&				getAllCubemap() { return m_cubemap_storage.GetAll(AssetType::SCENE); }
		const std::map<std::string, File*>&				getAllFiles() { return m_file_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, RenderTextureDepth*>&	getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Mesh*>&				getAllMesh() { return m_mesh_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, ShaderMaterial*>&		getAllShaderMaterial() { return m_shaderMaterial_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Material*>&			getAllMaterial() { return m_material_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(AssetType::SCENE); }
		const std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(AssetType::SCENE); }

	} SingletonInstance(SceneAssets);

	static class GlobalAssets : public Assets, public Singleton<class GlobalAssets>
	{
		DISALLOW_COPY_AND_ASSIGN(GlobalAssets);

		friend class RenderManager;
		void InitGLResources();

		// Generic Global Data
		TextureIndex texID_nullImageCheckerboard;
		TextureIndex texID_nullImageBlack;
		TextureIndex texID_nullImagePink;
		TextureIndex texID_editor_camera;
		TextureIndex texID_editor_light;

		ShaderMaterialIndex shader_error;
		MaterialIndex material_error;

	public:
		GlobalAssets()
		{
			m_creationType = AssetType::GLOBAL;
		}

		// Get All
		const std::map<uint32_t, BaseTexture*>&			getAllBaseTexture() { return m_baseTexture_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Cubemap*>&				getAllCubemap() { return m_cubemap_storage.GetAll(AssetType::GLOBAL); }
		const std::map<std::string, File*>&				getAllFiles() { return m_file_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, RenderTextureDepth*>&	getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Mesh*>&				getAllMesh() { return m_mesh_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, ShaderMaterial*>&		getAllShaderMaterial() { return m_shaderMaterial_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Material*>&			getAllMaterial() { return m_material_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(AssetType::GLOBAL); }
		const std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(AssetType::GLOBAL); }

		// Get Specific Global Data
		Texture2D* getTex2DNullImageCheckerboard(void) const { return m_texture2D_storage.Get(texID_nullImageCheckerboard); }
		Texture2D* getTex2DNullImageBlack(void) const { return m_texture2D_storage.Get(texID_nullImageBlack); }
		Texture2D* getTex2DNullImagePink(void) const { return m_texture2D_storage.Get(texID_nullImagePink); }
		Texture2D* getTex2DEditorCamera(void) const { return m_texture2D_storage.Get(texID_editor_camera); }
		Texture2D* getTex2DEditorLight(void) const { return m_texture2D_storage.Get(texID_editor_light); }
		
		Material* getMaterialError(void) const { return m_material_storage.Get(material_error); }

	} SingletonInstance(GlobalAssets);

}