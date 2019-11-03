// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <set>
#include <map>
#include <stack>
#include <vector>

#include "../math/Color.h"
#include "../utilities/Logger.h"
#include "../utilities/singleton.h"

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
	// Forward Declare Enums
	enum class TextureWrapping;
	enum class TextureFilter;
	enum class TextureFormat;
	enum class TexturePixelType;
	enum class TextureChannelType;
	enum class AnisotropicMode;
	enum class TextureDepthFormat;

	using Texture2DIndex = uint32_t;
	using CubemapIndex = uint32_t;
	using RenderTextureIndex = uint32_t;
	using RenderTextureDepthIndex = uint32_t;
	using RenderBufferIndex = uint32_t;
	using RenderBufferDepthIndex = uint32_t;

	template<class Type>
	class IDStorage
	{
	public:
		static uint32_t				m_nextID;
		std::map<uint32_t, Type*>	m_storage;
		std::stack<uint32_t>		m_deletedIDs;

		uint32_t	Add(Type* data)
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

			// Store and return ID
			m_storage[NewID] = data;
			return NewID;
		}
		Type*		Get(uint32_t id)
		{
			// return data
			if (m_storage.find(id) != m_storage.end())
				return m_storage[id];

			// not found
			return nullptr;
		}
		Type*		Erase(uint32_t id)
		{
			if (m_storage.find(id) != m_storage.end())
			{
				m_deletedIDs.push(id);
				Type* data = m_storage[id];
				m_storage.erase(id);
				return data;
			}
			return nullptr;
		}
		void		EraseAll(void)
		{
			for (auto& data : m_storage)
				m_deletedIDs.push(data.first);

			m_storage.clear();
		}
		std::map<uint32_t, Type*>& GetAll(void)
		{
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

		void	Add(const std::string& name, Type* data)
		{
			m_storage[name] = data;
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
		std::map<std::string, Type*>& GetAll(void)
		{
			return m_storage;
		}
	};

	class Assets
	{
		friend class RenderManager;
	protected:
		IDStorage<Texture2D> m_texture2D_storage;
		IDStorage<Cubemap> m_cubemap_storage;
		NamedStorage<File> m_file_storage;
		// Shader
		// Mesh
		IDStorage<RenderTexture> m_renderTexture_storage;
		IDStorage<RenderTextureDepth> m_renderTextureDepth_storage;
		// FBO
		IDStorage<RenderBuffer> m_renderBuffer_storage;
		IDStorage<RenderBufferDepth> m_renderBufferDepth_storage;
		// Entity

		// Protected
		Assets() {}

		void DestroyAndEraseAll();

	public:
		// Removers
		Texture2D*			eraseTexture2D(Texture2DIndex index) { m_texture2D_storage.Erase(index); }
		Cubemap*			eraseCubemap(CubemapIndex index) { m_cubemap_storage.Erase(index); }
		File*				eraseFile(const std::string& name) { m_file_storage.Erase(name); }
		RenderTexture*		eraseRenderTexture(RenderTextureIndex index) { m_renderTexture_storage.Erase(index); }
		RenderTextureDepth*	eraseRenderTextureDepth(RenderTextureDepthIndex index) { m_renderTextureDepth_storage.Erase(index); }
		RenderBuffer*		eraseRenderBuffer(RenderBufferIndex index) { m_renderBuffer_storage.Erase(index); }
		RenderBufferDepth*	eraseRenderBufferDepth(RenderBufferDepthIndex index) { m_renderBufferDepth_storage.Erase(index); }

		// Getters
		Texture2D*			getTexture2D(Texture2DIndex index) { return m_texture2D_storage.Get(index); }
		Cubemap*			getCubemap(CubemapIndex index) { return m_cubemap_storage.Get(index); }
		File*				getFile(const std::string& name) { return m_file_storage.Get(stringUtil::toLowerCopy(name)); }
		RenderTexture*		getRenderTexture(RenderTextureIndex index) { return m_renderTexture_storage.Get(index); }
		RenderTextureDepth*	getRenderTextureDepth(RenderTextureDepthIndex index) { return m_renderTextureDepth_storage.Get(index); }
		RenderBuffer*		getRenderBuffer(RenderBufferIndex index) { return m_renderBuffer_storage.Get(index); }
		RenderBufferDepth*	getRenderBufferDepth(RenderBufferDepthIndex index) { return m_renderBufferDepth_storage.Get(index); }

		// Get All
		std::map<uint32_t, Texture2D*>&			getAllTexture2D() { return m_texture2D_storage.GetAll(); }
		std::map<uint32_t, Cubemap*>&			getAllCubemap() { return m_cubemap_storage.GetAll(); }
		std::map<std::string, File*>&			getAllFiles() { return m_file_storage.GetAll(); }
		std::map<uint32_t, RenderTexture*>&		getAllRenderTexture() { return m_renderTexture_storage.GetAll(); }
		std::map<uint32_t, RenderTextureDepth*>&getAllRenderTextureDepth() { return m_renderTextureDepth_storage.GetAll(); }
		std::map<uint32_t, RenderBuffer*>&		getAllRenderBuffer() { return m_renderBuffer_storage.GetAll(); }
		std::map<uint32_t, RenderBufferDepth*>&	getAllRenderBufferDepth() { return m_renderBufferDepth_storage.GetAll(); }

		// Texture Load/create
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
		// Cubemap
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
		// File
		void loadFile(
			const std::string& name,
			const std::string& filepath
		);
		// Render Texture
		RenderTextureIndex createRenderTexture(
			int Width,
			int Height,
			TextureFormat FormatType,
			TexturePixelType PixelType,
			bool MipMapping
		);
		// Render Texture Depth
		RenderTextureDepthIndex createRenderTextureDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);
		// Render Buffer
		RenderBufferIndex createRenderBuffer(
			int Width,
			int Height,
			TextureFormat FormatType,
			TexturePixelType PixelType
		);
		// Render Buffer Depth
		RenderBufferDepthIndex createRenderBufferDepth(
			int Width,
			int Height,
			TextureDepthFormat FormatType
		);
	};

	static class GlobalAssets : public Assets, public Singleton<class GlobalAssets>
	{
	} SingletonInstance(GlobalAssets);

	static class SceneAssets : public Assets, public Singleton<class SceneAssets>
	{
	} SingletonInstance(SceneAssets);
	


	// Plan to make obsolete below //

	enum class AssetMessage
	{
		NONE,
		CREATED,
		LOADED
	};

	// Storage for objects with names
	template<class Type>
	class Asset
	{
		using UNNAMED_ASSETS = std::set<Type*>;
		using NAMED_ASSETS = std::map<std::string, Type*>;
	protected:
		// Protect Construction
		Asset() {}

		// No copy
		Asset(const Asset&) = delete;
		Asset<Type>& operator=(const Asset&) = delete;
		
		// Database
		static UNNAMED_ASSETS m_unnamed_assets;
		static NAMED_ASSETS m_named_assets;
		
		// If name already exists, append additional info on it to make it unique
		static std::string FixNameDuplicate(const std::string& name)
		{
			std::string Finalname = name;
			int DuplicateCounter = 0;
			// Add to name if duplicate found
			while (m_named_assets.find(Finalname) != m_named_assets.end())
				Finalname = name + " (" + std::to_string(DuplicateCounter++) + ')';
			// Return
			return Finalname;
		}
		// Add
		static void AddNamedAsset(const std::string& name, Type* object, AssetMessage message)
		{
			// Delete object with the same name
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				Logger.error("Duplicate [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				delete m_named_assets[name];
			}

			// Message
			switch (message)
			{
			case AssetMessage::CREATED:
				Logger.log("Created [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			case AssetMessage::LOADED:
				Logger.log("Loaded [" + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			}

			// Store
			m_named_assets[name] = object;
		}
		static void AddUnnamedAsset(Type* object, AssetMessage message)
		{
			std::ostringstream address;
			address << (void const *)object;
			std::string name = address.str();

			// Message
			switch (message)
			{
			case AssetMessage::CREATED:
			{
				Logger.log("Created Object, Ptr = " + name + "\t\t object: " + std::string(typeid(Type).name()));
				break;
			}
			case AssetMessage::LOADED:
				Logger.log("Loaded Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));
				break;
			}

			// Check for duplicate
			if(m_unnamed_assets.find(object) != m_unnamed_assets.end())
				Logger.error("Duplicate Object, Ptr = " + name + "]\t\t object: " + std::string(typeid(Type).name()));

			// Store
			m_unnamed_assets.insert(object);
		}
	public:

		// ~ Utility ~ //
		static bool CheckAsset(const std::string& name)
		{
			return (m_named_assets.find(name) != m_named_assets.end());
		}
		// Returns newName [ in case it had to modified such as "newName(0)" ]
		static std::string RenameAsset(const std::string& _oldName, const std::string& _newName)
		{
			VXL_ASSERT(CheckAsset(_oldName), "No Asset with name: " + _oldName);

			Type* _asset = m_named_assets[_oldName];
			m_named_assets.erase(_oldName);

			std::string newName = FixNameDuplicate(_newName);
			m_named_assets[newName] = _asset;
			return newName;
		}

		// ~ Asset Deletion ~ //
		static void DeleteNamedAsset(const std::string& name)
		{
			// Delete object if exists
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				auto object = m_named_assets[name];
				m_named_assets.erase(name);
				delete object;
			}
		}
		static void DeleteNamedAsset(Type* object)
		{
			if (!object)
				return;

			for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
			{
				if (it->second == object)
				{
					m_named_assets.erase(it->first);
					break;
				}
			}
			delete object;
		}
		static void DeleteUnnamedAsset(Type* object)
		{
			if (!object)
				return;

			if (m_unnamed_assets.find(object) != m_unnamed_assets.end())
			{
				m_unnamed_assets.erase(object);
				delete object;
			}
		}
		static void DeleteAllAssets(void)
		{
			for (auto it = m_named_assets.begin(); it != m_named_assets.end(); it++)
				delete it->second;

			for (auto it = m_unnamed_assets.begin(); it != m_unnamed_assets.end(); it++)
				delete *it;

			m_named_assets.clear();
			m_unnamed_assets.clear();
		}

		// ~ Get Asset Info ~ //
		static Type* GetAsset(const std::string name)
		{
			if (m_named_assets.find(name) != m_named_assets.end())
			{
				return m_named_assets[name];
			}
			return nullptr;
		}
		static NAMED_ASSETS GetAllNamedAssets(void)
		{
			return m_named_assets;
		}
		static size_t GetAssetCount(void)
		{
			return m_named_assets.size();
		}
	};

	template<class Type>
	std::set<Type*> Asset<Type>::m_unnamed_assets;
	template<class Type>
	std::map<std::string, Type*> Asset<Type>::m_named_assets;
}