// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <vector>
//#include <algorithm>

#include "../math/Color.h"
#include "../utilities/Logger.h"
#include "../utilities/singleton.h"
#include "../utilities/Macros.h"

// Used as a means of inheriting a unique map of objects //
namespace Vxl
{
	// Forward Declare Classes
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
	class _Shader;
	class _ShaderProgram;
	class _Material;
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

	using Texture2DIndex = uint32_t;
	using CubemapIndex = uint32_t;
	using FramebufferObjectIndex = uint32_t;
	using RenderTextureIndex = uint32_t;
	using RenderTextureDepthIndex = uint32_t;
	using RenderBufferIndex = uint32_t;
	using RenderBufferDepthIndex = uint32_t;
	using MeshIndex = uint32_t;
	using ShaderIndex = uint32_t;
	using ShaderProgramIndex = uint32_t;
	using MaterialIndex = uint32_t;
	using EntityIndex = uint32_t;
	using CameraIndex = uint32_t;

	enum class AssetType
	{
		GLOBAL,
		SCENE
	};

	template<class Type>
	class IDStorage
	{
	public:
		static uint32_t				m_nextID;
		std::stack<uint32_t>		m_deletedIDs;
		std::map<uint32_t, Type*>	m_storage;
		std::map<uint32_t, Type*>	m_storage_typeGlobal;
		std::map<uint32_t, Type*>	m_storage_typeScene;
		
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
			for (auto& data : m_storage)
				m_deletedIDs.push(data.first);

			m_storage_typeGlobal.clear();
			m_storage_typeScene.clear();
			m_storage.clear();
		}
		void		EraseAll(AssetType type)
		{
			if (type == AssetType::GLOBAL)
			{
				for (auto& data : m_storage_typeGlobal)
					m_deletedIDs.push(data.first);

				m_storage_typeGlobal.clear();
				m_storage = m_storage_typeScene;
			}
			else
			{
				for (auto& data : m_storage_typeScene)
					m_deletedIDs.push(data.first);

				m_storage_typeScene.clear();
				m_storage = m_storage_typeGlobal;
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
	uint32_t IDStorage<Type>::m_nextID = 0;

	template<class Type>
	class NamedStorage
	{
	public:
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
		static IDStorage<Texture2D>			 m_texture2D_storage;
		static IDStorage<Cubemap>			 m_cubemap_storage;
		static NamedStorage<File>			 m_file_storage;
		static IDStorage<FramebufferObject>	 m_framebufferObject_storage;
		static IDStorage<RenderTexture>		 m_renderTexture_storage;
		static IDStorage<RenderTextureDepth> m_renderTextureDepth_storage;
		static IDStorage<RenderBuffer>		 m_renderBuffer_storage;
		static IDStorage<RenderBufferDepth>	 m_renderBufferDepth_storage;
		static IDStorage<Mesh>				 m_mesh_storage;
		static IDStorage<_Shader>			 m_shader_storage;
		static IDStorage<_ShaderProgram>	 m_shaderProgram_storage;
		static IDStorage<_Material>			 m_material_storage;
		static IDStorage<Entity>			 m_entity_storage;
		static IDStorage<Camera>			 m_camera_storage;

		void DestroyAndEraseAll();

		AssetType m_creationType;

		// Protected
		Assets() {}

	public:
		// Removers
		static Texture2D*			eraseTexture2D(Texture2DIndex index) { return m_texture2D_storage.Erase(index); }
		static Cubemap*				eraseCubemap(CubemapIndex index) { return m_cubemap_storage.Erase(index); }
		static File*				eraseFile(const std::string& name) { return m_file_storage.Erase(name); }
		static FramebufferObject*	eraseFramebufferObject(FramebufferObjectIndex index) { return m_framebufferObject_storage.Erase(index);  }
		static RenderTexture*		eraseRenderTexture(RenderTextureIndex index) { return m_renderTexture_storage.Erase(index); }
		static RenderTextureDepth*	eraseRenderTextureDepth(RenderTextureDepthIndex index) { return m_renderTextureDepth_storage.Erase(index); }
		static RenderBuffer*		eraseRenderBuffer(RenderBufferIndex index) { return m_renderBuffer_storage.Erase(index); }
		static RenderBufferDepth*	eraseRenderBufferDepth(RenderBufferDepthIndex index) { return m_renderBufferDepth_storage.Erase(index); }
		static Mesh*				eraseMesh(MeshIndex index) { return m_mesh_storage.Erase(index); }
		static _Shader*				eraseShader(ShaderIndex index) { return m_shader_storage.Erase(index); }
		static _ShaderProgram*		eraseShaderProgram(ShaderProgramIndex index) { return m_shaderProgram_storage.Erase(index); }
		static _Material*			eraseMaterial(MaterialIndex index) { return m_material_storage.Erase(index); }
		static Entity*				eraseEntity(EntityIndex index) { return m_entity_storage.Erase(index); }
		static Camera*				eraseCamera(CameraIndex index) { return m_camera_storage.Erase(index); }

		// Delete [Erases too]
		static void deleteTexture2D(Texture2DIndex index);
		static void deleteCubemap(CubemapIndex index);
		static void deleteFile(const std::string& name);
		static void deleteFramebufferObject(FramebufferObjectIndex index);
		static void deleteRenderTexture(RenderTextureIndex index);
		static void deleteRenderTextureDepth(RenderTextureDepthIndex index);
		static void deleteRenderBuffer(RenderBufferIndex index);
		static void deleteRenderBufferDepth(RenderBufferDepthIndex index);
		static void deleteMesh(MeshIndex index);
		static void deleteShader(ShaderIndex index);
		static void deleteShaderProgram(ShaderProgramIndex index);
		static void deleteMaterial(MaterialIndex index);
		static void deleteEntity(EntityIndex index);
		static void deleteCamera(CameraIndex index);

		// Getters
		static Texture2D*			getTexture2D(Texture2DIndex index) { return m_texture2D_storage.Get(index); }
		static Cubemap*				getCubemap(CubemapIndex index) { return m_cubemap_storage.Get(index); }
		static File*				getFile(const std::string& name) { return m_file_storage.Get(stringUtil::toLowerCopy(name)); }
		static FramebufferObject*	getFramebufferObject(FramebufferObjectIndex index) { return m_framebufferObject_storage.Get(index); }
		static RenderTexture*		getRenderTexture(RenderTextureIndex index) { return m_renderTexture_storage.Get(index); }
		static RenderTextureDepth*	getRenderTextureDepth(RenderTextureDepthIndex index) { return m_renderTextureDepth_storage.Get(index); }
		static RenderBuffer*		getRenderBuffer(RenderBufferIndex index) { return m_renderBuffer_storage.Get(index); }
		static RenderBufferDepth*	getRenderBufferDepth(RenderBufferDepthIndex index) { return m_renderBufferDepth_storage.Get(index); }
		static Mesh*				getMesh(MeshIndex index) { return m_mesh_storage.Get(index); }
		static _Shader*				getShader(ShaderIndex index) { return m_shader_storage.Get(index); }
		static _ShaderProgram*		getShaderProgram(ShaderProgramIndex index) { return m_shaderProgram_storage.Get(index); }
		static _Material*			getMaterial(MaterialIndex index) { return m_material_storage.Get(index); }
		static Entity*				getEntity(EntityIndex index) { return m_entity_storage.Get(index); }
		static Camera*				getCamera(CameraIndex index) { return m_camera_storage.Get(index); }

		// Get All
		static std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(); }
		static std::map<uint32_t, Cubemap*>&			getAllCubemap() { return m_cubemap_storage.GetAll(); }
		static std::map<std::string, File*>&			getAllFiles() { return m_file_storage.GetAll(); }
		static std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(); }
		static std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(); }
		static std::map<uint32_t, RenderTextureDepth*>& getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(); }
		static std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(); }
		static std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(); }
		static std::map<uint32_t, Mesh*>&				getAllMesh() { return m_mesh_storage.GetAll(); }
		static std::map<uint32_t, _Shader*>&			getAllShader() { return m_shader_storage.GetAll(); }
		static std::map<uint32_t, _ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(); }
		static std::map<uint32_t, _Material*>&			getAllMaterial() { return m_material_storage.GetAll(); }
		static std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(); }
		static std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(); }

		//
		Texture2DIndex loadTexture2D(
			const std::string& filePath,
			bool				InvertY,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			TextureFormat		FormatType,
			TexturePixelType	PixelType,
			AnisotropicMode		AnisotropicMode
		);
		Texture2DIndex createTexture2D(
			std::vector<Color3F> pixels, uint32_t width,
			bool				UseMipMapping,
			TextureWrapping		WrapMode,
			TextureFilter		FilterMode,
			TextureFormat		FormatType,
			TextureChannelType	ChannelType,
			TexturePixelType	PixelType,
			AnisotropicMode		AnisotropicMode
		);
		Texture2DIndex createTexture2D(
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
		CubemapIndex loadCubemap(
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
		ShaderIndex createShader(const std::string& name, const std::string& sourceCode, ShaderType type);
		//
		ShaderProgramIndex createShaderProgram(const std::string& name, const std::vector<_Shader*>& _shaders);
		ShaderProgramIndex createShaderProgram(const std::string& name, std::initializer_list<_Shader*> _shaders);
		// 
		MaterialIndex createMaterial(const std::string& name);
		//
		EntityIndex createEntity(const std::string& name);
		//
		CameraIndex createCamera(const std::string& name, float znear = 0.1f, float zfar = 100.0f);
		
	
	};
	
	static class SceneAssets : public Assets, public Singleton<class SceneAssets>
	{
	public:
		SceneAssets()
		{
			m_creationType = AssetType::SCENE;
		}

		// Get All
		static std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, Cubemap*>&			getAllCubemap() { return m_cubemap_storage.GetAll(AssetType::SCENE); }
		static std::map<std::string, File*>&			getAllFiles() { return m_file_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, RenderTextureDepth*>& getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, Mesh*>&				getAllMesh() { return m_mesh_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, _Shader*>&			getAllShader() { return m_shader_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, _ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, _Material*>&			getAllMaterial() { return m_material_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(AssetType::SCENE); }
		static std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(AssetType::SCENE); }

	} SingletonInstance(SceneAssets);

	static class GlobalAssets : public Assets, public Singleton<class GlobalAssets>
	{
	public:
		GlobalAssets()
		{
			m_creationType = AssetType::SCENE;
		}

		// Get All
		static std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, Cubemap*>&			getAllCubemap() { return m_cubemap_storage.GetAll(AssetType::GLOBAL); }
		static std::map<std::string, File*>&			getAllFiles() { return m_file_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, FramebufferObject*>&	getAllFramebufferObject() { return m_framebufferObject_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, RenderTextureDepth*>& getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, Mesh*>&				getAllMesh() { return m_mesh_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, _Shader*>&			getAllShader() { return m_shader_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, _ShaderProgram*>&		getAllShaderProgram() { return m_shaderProgram_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, _Material*>&			getAllMaterial() { return m_material_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, Entity*>&				getAllEntity() { return m_entity_storage.GetAll(AssetType::GLOBAL); }
		static std::map<uint32_t, Camera*>&				getAllCamera() { return m_camera_storage.GetAll(AssetType::GLOBAL); }

	} SingletonInstance(GlobalAssets);

	//// Plan to make obsolete below //

	//enum class AssetMessage
	//{
	//	NONE,
	//	CREATED,
	//	LOADED
	//};

	//// Storage for objects with names
	//template<class Type>
	//class Asset
	//{
	//	using UNNAMED_ASSETS = std::set<Type*>;
	//	using NAMED_ASSETS = std::map<std::string, Type*>;
	//protected:
	//	// Protect Construction
	//	Asset() {}

	//	// No copy
	//	Asset(const Asset&) = delete;
	//	Asset<Type>& operator=(const Asset&) = delete;
	//	
	//	// Database
	//	static UNNAMED_ASSETS m_unnamed_assets;
	//	static NAMED_ASSETS m_named_assets;
	//	
	//	// If name already exists, append additional info on it to make it unique
	//	static std::string FixNameDuplicate(const std::string& name)
	//	{
	//		std::string Finalname = name;
	//		int DuplicateCounter = 0;
	//		// Add to name if duplicate found
	//		while (m_named_assets.find(Finalname) != m_named_assets.end())
	//			Finalname = name + " (" + std::to_string(DuplicateCounter++) + ')';
	//		// Return
	//		return Finalname;
	//	}
	//	// Add
	//	static void AddNamedAsset(const std::string& name, Type* object, AssetMessage message)
	//	{
	//		// Delete object with the same name
	//		if (m_named_assets.find(name) != m_named_assets.end())
	//		{
	//			Logger.error("Duplicate [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
	//			delete m_named_assets[name];
	//		}

	//		// Message
	//		switch (message)
	//		{
	//		case AssetMessage::CREATED:
	//			Logger.log("Created [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
	//			break;
	//		case AssetMessage::LOADED:
	//			Logger.log("Loaded [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
	//			break;
	//		}

	//		// Store
	//		m_named_assets[name] = object;
	//	}
	//	static void AddUnnamedAsset(Type* object, AssetMessage message)
	//	{
	//		std::ostringstream address;
	//		address << (void const *)object;
	//		std::string name = address.str();

	//		// Message
	//		switch (message)
	//		{
	//		case AssetMessage::CREATED:
	//		{
	//			Logger.log("Created Object, Ptr = " + name + "\t\t object: " + std::string(typeid(Type).name()));
	//			break;
	//		}
	//		case AssetMessage::LOADED:
	//			Logger.log("Loaded Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));
	//			break;
	//		}

	//		// Check for duplicate
	//		if(m_unnamed_assets.find(object) != m_unnamed_assets.end())
	//			Logger.error("Duplicate Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));

	//		// Store
	//		m_unnamed_assets.insert(object);
	//	}
	//public:

	//	// ~ Utility ~ //
	//	static bool CheckAsset(const std::string& name)
	//	{
	//		return (m_named_assets.find(name) != m_named_assets.end());
	//	}
	//	// Returns newName [ in case it had to modified such as "newName(0)" ]
	//	static std::string RenameAsset(const std::string& _oldName, const std::string& _newName)
	//	{
	//		VXL_ASSERT(CheckAsset(_oldName), "No Asset with name: " + _oldName);

	//		Type* _asset = m_named_assets[_oldName];
	//		m_named_assets.erase(_oldName);

	//		std::string newName = FixNameDuplicate(_newName);
	//		m_named_assets[newName] = _asset;
	//		return newName;
	//	}

	//	// ~ Asset Deletion ~ //
	//	static void DeleteNamedAsset(const std::string& name)
	//	{
	//		// Delete object if exists
	//		if (m_named_assets.find(name) != m_named_assets.end())
	//		{
	//			auto object = m_named_assets[name];
	//			m_named_assets.erase(name);
	//			delete object;
	//		}
	//	}
	//	static void DeleteNamedAsset(Type* object)
	//	{
	//		if (!object)
	//			return;

	//		for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
	//		{
	//			if (it->second == object)
	//			{
	//				m_named_assets.erase(it->first);
	//				break;
	//			}
	//		}
	//		delete object;
	//	}
	//	static void DeleteUnnamedAsset(Type* object)
	//	{
	//		if (!object)
	//			return;

	//		if (m_unnamed_assets.find(object) != m_unnamed_assets.end())
	//		{
	//			m_unnamed_assets.erase(object);
	//			delete object;
	//		}
	//	}
	//	static void DeleteAllAssets(void)
	//	{
	//		for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
	//			delete it->second;

	//		for (auto it = m_unnamed_assets.begin(); it != m_unnamed_assets.end(); it++)
	//			delete *it;

	//		m_named_assets.clear();
	//		m_unnamed_assets.clear();
	//	}

	//	// ~ Get Asset Info ~ //
	//	static Type* GetAsset(const std::string name)
	//	{
	//		if (m_named_assets.find(name) != m_named_assets.end())
	//		{
	//			return m_named_assets[name];
	//		}
	//		return nullptr;
	//	}
	//	static NAMED_ASSETS GetAllNamedAssets(void)
	//	{
	//		return m_named_assets;
	//	}
	//	static size_t GetAssetCount(void)
	//	{
	//		return m_named_assets.size();
	//	}
	//};

	//template<class Type>
	//std::set<Type*> Asset<Type>::m_unnamed_assets;
	//template<class Type>
	//std::map<std::string, Type*> Asset<Type>::m_named_assets;
}