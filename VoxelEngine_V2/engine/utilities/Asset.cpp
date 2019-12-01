// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Asset.h"

#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../textures/Cubemap.h"
#include "../utilities/FileIO.h"
#include "../rendering/RenderBuffer.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Mesh.h"
#include "../rendering/Shader.h"
#include "../modules/Material.h"
#include "../modules/Entity.h"
#include "../objects/Camera.h"

namespace Vxl
{
	IDStorage<BaseTexture>		  Assets::m_baseTexture_storage;
	IDStorage<Texture2D>		  Assets::m_texture2D_storage;
	IDStorage<Cubemap>			  Assets::m_cubemap_storage;
	NamedStorage<File>			  Assets::m_file_storage;
	IDStorage<FramebufferObject>  Assets::m_framebufferObject_storage;
	IDStorage<RenderTexture>	  Assets::m_renderTexture_storage;
	IDStorage<RenderTextureDepth> Assets::m_renderTextureDepth_storage;
	IDStorage<RenderBuffer>		  Assets::m_renderBuffer_storage;
	IDStorage<RenderBufferDepth>  Assets::m_renderBufferDepth_storage;
	IDStorage<Mesh>				  Assets::m_mesh_storage;
	IDStorage<ShaderProgram>	  Assets::m_shaderProgram_storage;
	IDStorage<ShaderMaterial>	  Assets::m_shaderMaterial_storage;
	IDStorage<Material>			  Assets::m_material_storage;
	IDStorage<Entity>			  Assets::m_entity_storage;
	IDStorage<Camera>			  Assets::m_camera_storage;

	void GlobalAssets::InitGLResources()
	{
		// Load files
		loadFile("_UBO.glsl", "./assets/files/_UBO.glsl");
		loadFile("_Math.glsl", "./assets/files/_Math.glsl");
		loadFile("_Uniforms.glsl", "./assets/files/_Uniforms.glsl");

		// Editor Textures
		texID_editor_camera = loadTexture2D(
			"./assets/textures/editor_camera.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGBA8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);

		//
		texID_editor_light = loadTexture2D(
			"./assets/textures/editor_lightbulb.png",
			true,
			true,
			TextureWrapping::REPEAT,
			TextureFilter::LINEAR,
			TextureFormat::RGBA8,
			TexturePixelType::UNSIGNED_BYTE,
			AnisotropicMode::NONE
		);

		// Debug Texture - checkerboard
		{
			const std::vector<Color3F> pixels = {
				Color3F::PURPLE,
				Color3F::BLACK,
				Color3F::PURPLE,
				Color3F::BLACK,

				Color3F::BLACK,
				Color3F::PURPLE,
				Color3F::BLACK,
				Color3F::PURPLE,

				Color3F::PURPLE,
				Color3F::BLACK,
				Color3F::PURPLE,
				Color3F::BLACK,

				Color3F::BLACK,
				Color3F::PURPLE,
				Color3F::BLACK,
				Color3F::PURPLE
			};

			texID_nullImageCheckerboard = createTexture2D(
				pixels, 4, true,
				TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST, AnisotropicMode::NONE
			);
		}
		// Debug texture - black
		{
			const std::vector<Color3F> pixels = {
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK,
				Color3F::BLACK
			};

			texID_nullImageBlack = createTexture2D(
				pixels, 4, false,
				TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST, AnisotropicMode::NONE
			);
		}
		// Debug textures - magenta
		{
			const std::vector<Color3F> pixels = {
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK,
				Color3F::PINK
			};

			texID_nullImagePink = createTexture2D(
				pixels, 4, false,
				TextureWrapping::CLAMP_STRETCH, TextureFilter::NEAREST, AnisotropicMode::NONE
			);
		}
		// Error Material
		{
			shaderMaterial_error = createShaderMaterial("./assets/materials/error.material");
			material_error = createMaterial("error");
			{
				auto mat = Assets::getMaterial(material_error);
				mat->setShaderMaterial(shaderMaterial_error);
				mat->setSequenceID(999999u);
			}
		}
		// Show Render Target
		{
			shader_showRenderTarget = SceneAssets.createShaderMaterial("./assets/materials/displayRenderTarget.material");
		}
	}
	ShaderProgram* GlobalAssets::getShader_ShowRenderTarget(void) const
	{
		ShaderMaterial* shadMat = m_shaderMaterial_storage.Get(shader_showRenderTarget);
		if (shadMat)
			return m_shaderProgram_storage.Get(shadMat->m_coreProgram);

		return nullptr;
	}

	void Assets::DestroyAndEraseAll()
	{
		// Delete All Textures Here
		auto& textures = m_baseTexture_storage.GetAll(m_creationType);
		for (auto& texture : textures)
			delete texture.second;

		auto& files = m_file_storage.GetAll(m_creationType);
		for (auto& file : files)
			delete file.second;

		auto& fbos = m_framebufferObject_storage.GetAll(m_creationType);
		for (auto& fbo : fbos)
			delete fbo.second;

		auto& renderTexture = m_renderTexture_storage.GetAll(m_creationType);
		for (auto& rt : renderTexture)
			delete rt.second;

		auto& renderTextureDepth = m_renderTextureDepth_storage.GetAll(m_creationType);
		for (auto& rtd : renderTextureDepth)
			delete rtd.second;

		auto& renderBuffer = m_renderBuffer_storage.GetAll(m_creationType);
		for (auto& rb : renderBuffer)
			delete rb.second;

		auto& renderBufferDepth = m_renderBufferDepth_storage.GetAll(m_creationType);
		for (auto& rbd : renderBufferDepth)
			delete rbd.second;

		auto& meshes = m_mesh_storage.GetAll(m_creationType);
		for (auto& mesh : meshes)
			delete mesh.second;

		auto& shaderPrograms = m_shaderProgram_storage.GetAll(m_creationType);
		for (auto& sp : shaderPrograms)
			delete sp.second;

		auto& shaderMaterials = m_shaderMaterial_storage.GetAll(m_creationType);
		for (auto& sm : shaderMaterials)
			delete sm.second;

		auto& materials = m_material_storage.GetAll(m_creationType);
		for (auto& mat : materials)
			delete mat.second;

		auto& entities = m_entity_storage.GetAll(m_creationType);
		for (auto& entity : entities)
			delete entity.second;

		auto& cameras = m_camera_storage.GetAll(m_creationType);
		for (auto& cam : cameras)
			delete cam.second;

		// erase All Assets
		m_baseTexture_storage.EraseAll(m_creationType);
		m_texture2D_storage.EraseAll(m_creationType);
		m_cubemap_storage.EraseAll(m_creationType);
		m_file_storage.EraseAll(m_creationType);
		m_framebufferObject_storage.EraseAll(m_creationType);
		m_renderTexture_storage.EraseAll(m_creationType);
		m_renderTextureDepth_storage.EraseAll(m_creationType);
		m_renderBuffer_storage.EraseAll(m_creationType);
		m_renderBufferDepth_storage.EraseAll(m_creationType);
		m_mesh_storage.EraseAll(m_creationType);
		m_shaderProgram_storage.EraseAll(m_creationType);
		m_shaderMaterial_storage.EraseAll(m_creationType);
		m_material_storage.EraseAll(m_creationType);
		m_entity_storage.EraseAll(m_creationType);
		m_camera_storage.EraseAll(m_creationType);
	}
	// Deletion
	BaseTexture* Assets::eraseBaseTexture(TextureIndex index)
	{
		// Attempt to erase the texture index from all texture slots
		m_texture2D_storage.Erase(index);
		m_cubemap_storage.Erase(index);

		return m_baseTexture_storage.Erase(index);
	}

	void Assets::deleteBaseTexture(TextureIndex index)
	{
		// Attempt to erase the texture index from all texture slots
		m_texture2D_storage.Erase(index);
		m_cubemap_storage.Erase(index);

		delete m_baseTexture_storage.Erase(index);
	}
	void Assets::deleteTexture2D(TextureIndex index)
	{
		delete m_baseTexture_storage.Erase(index);
		delete m_texture2D_storage.Erase(index);
	}
	void Assets::deleteCubemap(TextureIndex index)
	{
		delete m_baseTexture_storage.Erase(index);
		delete m_cubemap_storage.Erase(index);
	}
	void Assets::deleteFile(const std::string& name)
	{
		delete m_file_storage.Erase(name);
	}
	void Assets::deleteFramebufferObject(FramebufferObjectIndex index)
	{
		delete m_framebufferObject_storage.Erase(index);
	}
	void Assets::deleteRenderTexture(RenderTextureIndex index)
	{
		delete m_renderTexture_storage.Erase(index);
	}
	void Assets::deleteRenderTextureDepth(RenderTextureDepthIndex index)
	{
		delete m_renderTextureDepth_storage.Erase(index);
	}
	void Assets::deleteRenderBuffer(RenderBufferIndex index)
	{
		delete m_renderBuffer_storage.Erase(index);
	}
	void Assets::deleteRenderBufferDepth(RenderBufferDepthIndex index)
	{
		delete m_renderBufferDepth_storage.Erase(index);
	}
	void Assets::deleteMesh(MeshIndex index)
	{
		delete m_mesh_storage.Erase(index);
	}
	void Assets::deleteShaderProgram(ShaderProgramIndex index)
	{
		delete m_shaderProgram_storage.Erase(index);
	}
	void Assets::deleteShaderMaterial(ShaderMaterialIndex index)
	{
		delete m_shaderMaterial_storage.Erase(index);
	}
	void Assets::deleteMaterial(MaterialIndex index)
	{
		delete m_material_storage.Erase(index);
	}
	void Assets::deleteEntity(EntityIndex index)
	{
		delete m_entity_storage.Erase(index);
	}
	void Assets::deleteCamera(CameraIndex index)
	{
		delete m_camera_storage.Erase(index);
	}

	void Assets::deleteAllTextures()
	{
		auto& textures = m_baseTexture_storage.GetAll();
		for (auto& texture : textures)
			delete texture.second;
		m_baseTexture_storage.EraseAll();
		m_texture2D_storage.EraseAll();
		m_cubemap_storage.EraseAll();
	}
	void Assets::deleteAllFile()
	{
		auto& files = m_file_storage.GetAll();
		for (auto& file : files)
			delete file.second;

		m_file_storage.EraseAll();
	}
	void Assets::deleteAllFramebufferObject()
	{
		auto& fbos = m_framebufferObject_storage.GetAll();
		for (auto& fbo : fbos)
			delete fbo.second;

		m_framebufferObject_storage.EraseAll();
	}
	void Assets::deleteAllRenderTexture()
	{
		auto& rts = m_renderTexture_storage.GetAll();
		for (auto& rt : rts)
			delete rt.second;

		m_renderTexture_storage.EraseAll();
	}
	void Assets::deleteAllRenderTextureDepth()
	{
		auto& rts = m_renderTextureDepth_storage.GetAll();
		for (auto& rt : rts)
			delete rt.second;

		m_renderTextureDepth_storage.EraseAll();
	}
	void Assets::deleteAllRenderBuffer()
	{
		auto& rbs = m_renderBuffer_storage.GetAll();
		for (auto& rb : rbs)
			delete rb.second;

		m_renderBuffer_storage.EraseAll();
	}
	void Assets::deleteAllRenderBufferDepth()
	{
		auto& rbs = m_renderBufferDepth_storage.GetAll();
		for (auto& rb : rbs)
			delete rb.second;

		m_renderBufferDepth_storage.EraseAll();
	}
	void Assets::deleteAllMesh()
	{
		auto& Meshes = m_mesh_storage.GetAll();
		for (auto& mesh : Meshes)
			delete mesh.second;

		m_mesh_storage.EraseAll();
	}
	void Assets::deleteAllShaderProgram()
	{
		auto& shaderPrograms = m_shaderProgram_storage.GetAll();
		for (auto& sp : shaderPrograms)
			delete sp.second;

		m_shaderProgram_storage.EraseAll();
	}
	void Assets::deleteAllShaderMaterial()
	{
		auto& shaderMaterials = m_shaderMaterial_storage.GetAll();
		for (auto& sm : shaderMaterials)
			delete sm.second;

		m_shaderMaterial_storage.EraseAll();
	}
	void Assets::deleteAllMaterial()
	{
		auto& materials = m_material_storage.GetAll();
		for (auto& mat : materials)
			delete mat.second;

		m_material_storage.EraseAll();
	}
	void Assets::deleteAllEntity()
	{
		auto& entities = m_entity_storage.GetAll();
		for (auto& ent : entities)
			delete ent.second;

		m_material_storage.EraseAll();
	}
	void Assets::deleteAllCamera()
	{
		auto& cameras = m_camera_storage.GetAll();
		for (auto& cam : cameras)
			delete cam.second;

		m_camera_storage.EraseAll();
	}

	// Creation
	TextureIndex Assets::loadTexture2D(
		const std::string& filePath,
		bool				InvertY,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New Data
		Texture2D* _texture = new Texture2D(filePath, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_texture, m_creationType);
		m_texture2D_storage.AddCustom(_texture, m_creationType, index);
		// Return index
		return index;
	}
	TextureIndex Assets::createTexture2D(
		std::vector<float> pixels, uint32_t width,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		AnisotropicMode		AnisotropicMode
	)
	{
		//VXL_ASSERT(pixels.size() >= 16, "Custom Texture must be at least 4x4 in size");
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, UseMipMapping, WrapMode, FilterMode, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_texture, m_creationType);
		m_texture2D_storage.AddCustom(_texture, m_creationType, index);
		// Return index
		return index;
	}
	TextureIndex Assets::createTexture2D(
		std::vector<Color3F> pixels, uint32_t width,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		AnisotropicMode		AnisotropicMode
	)
	{
		VXL_ASSERT(pixels.size() >= 16, "Custom Texture must be at least 4x4 in size");
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, UseMipMapping, WrapMode, FilterMode, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_texture, m_creationType);
		m_texture2D_storage.AddCustom(_texture, m_creationType, index);
		// Return index
		return index;
	}
	TextureIndex Assets::createTexture2D(
		std::vector<Color4F> pixels, uint32_t width,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		AnisotropicMode		AnisotropicMode
	)
	{
		VXL_ASSERT(pixels.size() >= 16, "Custom Texture must be at least 4x4 in size");
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, UseMipMapping, WrapMode, FilterMode, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_texture, m_creationType);
		m_texture2D_storage.AddCustom(_texture, m_creationType, index);
		// Return index
		return index;
	}
	TextureIndex Assets::createTexture2D(
		void* pixels, uint32_t width, uint32_t height,
		bool				UseMipMapping,
		TextureWrapping		WrapMode,
		TextureFilter		FilterMode,
		TextureFormat		FormatType,
		TextureChannelType	ChannelType,
		TexturePixelType	PixelType,
		AnisotropicMode		AnisotropicMode
	)
	{
		// Create New Data
		Texture2D* _texture = new Texture2D(pixels, width, height, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_texture, m_creationType);
		m_texture2D_storage.AddCustom(_texture, m_creationType, index);
		// Return index
		return index;
	}
	TextureIndex Assets::loadCubemap(
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
	)
	{
		// Create New Data
		Cubemap* _cubemap = new Cubemap(filePath1, filePath2, filePath3, filePath4, filePath5, filePath6, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);
		// Store Data
		TextureIndex index = m_baseTexture_storage.Add(_cubemap, m_creationType);
		m_cubemap_storage.AddCustom(_cubemap, m_creationType, index);
		// Return index
		return index;
	}
	void Assets::loadFile(
		const std::string& name,
		const std::string& filepath
	)
	{
		// Create New Data
		File* _file = new File(filepath);
		// Store Data and Return index
		m_file_storage.Add(stringUtil::toLowerCopy(name), _file, m_creationType);
	}
	FramebufferObjectIndex Assets::createFramebuffer(
		const std::string& name
	)
	{
		// Create New Data
		FramebufferObject* _fbo = new FramebufferObject(name);
		// Store Data and Return index
		return m_framebufferObject_storage.Add(_fbo, m_creationType);
	}
	RenderTextureIndex Assets::createRenderTexture(
		const std::string& name,
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType,
		bool MipMapping
	)
	{
		// Create New Data
		RenderTexture* _texture = new RenderTexture(name, Width, Height, FormatType, PixelType, MipMapping);
		// Store Data and Return index
		return m_renderTexture_storage.Add(_texture, m_creationType);
	}
	RenderTextureDepthIndex Assets::createRenderTextureDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
	{
		// Create New Data
		RenderTextureDepth* _texture = new RenderTextureDepth(Width, Height, FormatType);
		// Store Data and Return index
		return m_renderTextureDepth_storage.Add(_texture, m_creationType);
	}
	RenderBufferIndex Assets::createRenderBuffer(
		const std::string& name,
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
	{
		// Create New Data
		RenderBuffer* _buffer = new RenderBuffer(name, Width, Height, FormatType, PixelType);
		// Store Data and Return index
		return m_renderBuffer_storage.Add(_buffer, m_creationType);
	}
	RenderBufferDepthIndex Assets::createRenderBufferDepth(
		int Width,
		int Height,
		TextureDepthFormat FormatType
	)
	{
		// Create New Data
		RenderBufferDepth* _buffer = new RenderBufferDepth(Width, Height, FormatType);
		// Store Data and Return index
		return m_renderBufferDepth_storage.Add(_buffer, m_creationType);
	}
	MeshIndex Assets::createMesh()
	{
		// Create New Data
		Mesh* _mesh = new Mesh();
		// Store Data and Return index
		return m_mesh_storage.Add(_mesh, m_creationType);
	}

	ShaderProgramIndex Assets::createShaderProgram(const std::string& name, const std::vector<Shader*>& _shaders)
	{
		// Create New Data
		ShaderProgram* _program = new ShaderProgram(name, _shaders);
		// Store Data and Return index
		return m_shaderProgram_storage.Add(_program, m_creationType);
	}
	ShaderProgramIndex Assets::createShaderProgram(const std::string& name, std::initializer_list<Shader*> _shaders)
	{
		// Create New Data
		ShaderProgram* _program = new ShaderProgram(name, _shaders);
		// Store Data and Return index
		return m_shaderProgram_storage.Add(_program, m_creationType);
	}

	MaterialIndex Assets::createMaterial(const std::string& name)
	{
		// Create New Data
		Material* material = new Material(name);
		// Store Data and Return index
		MaterialIndex materialIndex = m_material_storage.Add(material, m_creationType);

		// Return Set info
		return materialIndex;
	}

	EntityIndex Assets::createEntity(const std::string& name)
	{
		// Create New Data
		Entity* object = new Entity(name);
		// Store Data
		EntityIndex index	= m_entity_storage.Add(object, m_creationType);
		object->m_uniqueID	= index;
		object->m_colorID	= Util::Conversion::uint_to_color4(object->m_uniqueID);
		// Return index
		return index;
	}
	CameraIndex Assets::createCamera(const std::string& name, float znear, float zfar)
	{
		// Create New Data
		Camera* object = new Camera(name, znear, zfar);
		object->m_transform.m_rotationOrder = EulerRotationOrder::YXZ;
		// Store Data and Return index
		return m_camera_storage.Add(object, m_creationType);
	}
	
	ShaderMaterialIndex Assets::createShaderMaterial(const std::string& filePath)
	{
		// Create New Data
		ShaderMaterial* object = new ShaderMaterial(filePath);
		// Store Data and Return index
		return m_shaderMaterial_storage.Add(object, m_creationType);
	}
}