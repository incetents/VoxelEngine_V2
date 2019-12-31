// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Asset.h"

#include "../modules/Material.h"
#include "../modules/Entity.h"
#include "../objects/Camera.h"
#include "../rendering/RenderBuffer.h"
#include "../rendering/FramebufferObject.h"
#include "../rendering/Mesh.h"
#include "../rendering/Shader.h"
#include "../rendering/RenderManager.h"
#include "../textures/Texture2D.h"
#include "../textures/RenderTexture.h"
#include "../textures/Cubemap.h"
#include "../utilities/FileIO.h"

namespace Vxl
{
	IDStorage<BaseTexture>		  _Assets::m_baseTexture_storage;
	IDStorage<Texture2D>		  _Assets::m_texture2D_storage;
	IDStorage<Cubemap>			  _Assets::m_cubemap_storage;
	NamedStorage<File>			  _Assets::m_file_storage;
	IDStorage<FramebufferObject>  _Assets::m_framebufferObject_storage;
	IDStorage<RenderTexture>	  _Assets::m_renderTexture_storage;
	IDStorage<RenderTextureDepth> _Assets::m_renderTextureDepth_storage;
	IDStorage<RenderBuffer>		  _Assets::m_renderBuffer_storage;
	IDStorage<RenderBufferDepth>  _Assets::m_renderBufferDepth_storage;
	IDStorage<Mesh>				  _Assets::m_mesh_storage;
	IDStorage<LineMesh3D>		  _Assets::m_lineMesh3D_storage;
	IDStorage<LineMesh2D>		  _Assets::m_lineMesh2D_storage;
	IDStorage<Shader>			  _Assets::m_shader_storage;
	IDStorage<ShaderProgram>	  _Assets::m_shaderProgram_storage;
	IDStorage<ShaderMaterial>	  _Assets::m_shaderMaterial_storage;
	IDStorage<Material>			  _Assets::m_material_storage;
	IDStorage<Entity>			  _Assets::m_entity_storage;
	IDStorage<Camera>			  _Assets::m_camera_storage;
	IDStorage<SceneNode>		  _Assets::m_sceneNode_storage;

	void GlobalAssets::InitGLResources()
	{
		// Load files
		loadFile("_Core.glsl", "./assets/files/_Core.glsl");
		loadFile("_Math.glsl", "./assets/files/_Math.glsl");

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
				auto mat = _Assets::getMaterial(material_error);
				mat->setShaderMaterial(shaderMaterial_error);
				mat->setSequenceID(9000);
			}
		}
		// Show Render Target
		{
			shader_showRenderTarget = createShaderMaterial("./assets/materials/displayRenderTarget.material");
		}
		// Line Render Material
		{
			shader_lineRender = createShaderMaterial("./assets/materials/lineRender.material");
		}
		// Simple Light Material
		{
			shader_debugRender = createShaderMaterial("./assets/materials/debugRender.material");
			material_debugRender = createMaterial("debugRender");
			{
				auto mat = _Assets::getMaterial(material_debugRender);
				mat->setShaderMaterial(shader_debugRender);
				mat->setSequenceID(9001);
				mat->m_blendFunc.source = BlendSource::ONE;
				mat->m_blendFunc.destination = BlendDestination::ZERO;
			}
		}
		// Passthrough Material
		{
			shader_passthrough = createShaderMaterial("./assets/materials/passthrough.material");
		}
	}
	ShaderProgram* GlobalAssets::get_ProgramShowRenderTarget(void) const
	{
		ShaderMaterial* shadMat = m_shaderMaterial_storage.Get(shader_showRenderTarget);
		if (shadMat)
			return m_shaderProgram_storage.Get(shadMat->m_coreProgram);

		return nullptr;
	}
	ShaderProgram* GlobalAssets::get_ProgramLineRender(void) const
	{
		ShaderMaterial* shadMat = m_shaderMaterial_storage.Get(shader_lineRender);
		if (shadMat)
			return m_shaderProgram_storage.Get(shadMat->m_coreProgram);

		return nullptr;
	}
	ShaderProgram* GlobalAssets::get_ProgramDebugRender(void) const
	{
		ShaderMaterial* shadMat = m_shaderMaterial_storage.Get(shader_debugRender);
		if (shadMat)
			return m_shaderProgram_storage.Get(shadMat->m_coreProgram);

		return nullptr;
	}
	ShaderProgram* GlobalAssets::get_ProgramPassthrough(void) const
	{
		ShaderMaterial* shadMat = m_shaderMaterial_storage.Get(shader_passthrough);
		if (shadMat)
			return m_shaderProgram_storage.Get(shadMat->m_coreProgram);

		return nullptr;
	}

	void _Assets::DestroyAndEraseAll()
	{
		// Delete All Textures Here
		auto& textures = m_baseTexture_storage.GetAll(m_creationType);
		for (auto& texture : textures)
			delete texture.second;

		// Can ignore Texture2D and Cubemap since they are part of textures

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

		auto& lineMeshes3D = m_lineMesh3D_storage.GetAll(m_creationType);
		for (auto& mesh : lineMeshes3D)
			delete mesh.second;

		auto& lineMeshes2D = m_lineMesh2D_storage.GetAll(m_creationType);
		for (auto& mesh : lineMeshes2D)
			delete mesh.second;

		auto& shaders = m_shader_storage.GetAll(m_creationType);
		for (auto& sp : shaders)
			delete sp.second;

		auto& shaderPrograms = m_shaderProgram_storage.GetAll(m_creationType);
		for (auto& sp : shaderPrograms)
			delete sp.second;

		auto& shaderMaterials = m_shaderMaterial_storage.GetAll(m_creationType);
		for (auto& sm : shaderMaterials)
			delete sm.second;

		auto& materials = m_material_storage.GetAll(m_creationType);
		for (auto& mat : materials)
			delete mat.second;

		auto& nodes = m_sceneNode_storage.GetAll(m_creationType);
		for (auto& node : nodes)
			delete node.second;

		// Can ignore entities and cameras since they are part of SceneNodes

		// erase All _Assets
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
		m_lineMesh3D_storage.EraseAll(m_creationType);
		m_lineMesh2D_storage.EraseAll(m_creationType);
		m_shader_storage.EraseAll(m_creationType);
		m_shaderProgram_storage.EraseAll(m_creationType);
		m_shaderMaterial_storage.EraseAll(m_creationType);
		m_material_storage.EraseAll(m_creationType);
		m_sceneNode_storage.EraseAll(m_creationType);
		m_entity_storage.EraseAll(m_creationType);
		m_camera_storage.EraseAll(m_creationType);
	}
	// Deletion
	BaseTexture* _Assets::eraseBaseTexture(TextureIndex index)
	{
		// Attempt to erase the texture index from all texture slots
		m_texture2D_storage.Erase(index);
		m_cubemap_storage.Erase(index);

		return m_baseTexture_storage.Erase(index);
	}

	void _Assets::deleteBaseTexture(TextureIndex index)
	{
		// Attempt to erase the texture index from all texture slots
		m_texture2D_storage.Erase(index);
		m_cubemap_storage.Erase(index);

		delete m_baseTexture_storage.Erase(index);
	}
	void _Assets::deleteTexture2D(TextureIndex index)
	{
		delete m_baseTexture_storage.Erase(index);
		delete m_texture2D_storage.Erase(index);
	}
	void _Assets::deleteCubemap(TextureIndex index)
	{
		delete m_baseTexture_storage.Erase(index);
		delete m_cubemap_storage.Erase(index);
	}
	void _Assets::deleteFile(const std::string& name)
	{
		delete m_file_storage.Erase(name);
	}
	void _Assets::deleteFramebufferObject(FramebufferObjectIndex index)
	{
		delete m_framebufferObject_storage.Erase(index);
	}
	void _Assets::deleteRenderTexture(RenderTextureIndex index)
	{
		delete m_renderTexture_storage.Erase(index);
	}
	void _Assets::deleteRenderTextureDepth(RenderTextureDepthIndex index)
	{
		delete m_renderTextureDepth_storage.Erase(index);
	}
	void _Assets::deleteRenderBuffer(RenderBufferIndex index)
	{
		delete m_renderBuffer_storage.Erase(index);
	}
	void _Assets::deleteRenderBufferDepth(RenderBufferDepthIndex index)
	{
		delete m_renderBufferDepth_storage.Erase(index);
	}
	void _Assets::deleteMesh(MeshIndex index)
	{
		delete m_mesh_storage.Erase(index);
	}
	void _Assets::deleteLineMesh3D(LineMesh3DIndex index)
	{
		delete m_lineMesh3D_storage.Erase(index);
	}
	void _Assets::deleteLineMesh2D(LineMesh2DIndex index)
	{
		delete m_lineMesh2D_storage.Erase(index);
	}
	void _Assets::deleteShaderProgram(ShaderProgramIndex index)
	{
		delete m_shaderProgram_storage.Erase(index);
	}
	void _Assets::deleteShaderMaterial(ShaderMaterialIndex index)
	{
		delete m_shaderMaterial_storage.Erase(index);
	}
	void _Assets::deleteMaterial(MaterialIndex index)
	{
		delete m_material_storage.Erase(index);
	}
	void _Assets::deleteSceneNode(SceneNodeIndex index)
	{
		SceneNode* node = m_sceneNode_storage.Get(index);
		if (node)
		{
			switch (node->m_type)
			{
			case SceneNodeType::ENTITY:
				m_entity_storage.Erase(index);
				break;
		
			case SceneNodeType::CAMERA:
				m_camera_storage.Erase(index);
				break;
			}
			delete m_sceneNode_storage.Erase(index);
		}

		// RenderManager re-sort
		RenderManager.dirtyEntitySequence();
	}
	void _Assets::deleteEntity(EntityIndex index)
	{
		// Remove Node
		m_sceneNode_storage.Erase(index);
		
		delete m_entity_storage.Erase(index);
		
		// RenderManager re-sort
		RenderManager.dirtyEntitySequence();
	}
	void _Assets::deleteCamera(CameraIndex index)
	{
		// Remove Node
		m_sceneNode_storage.Erase(index);
		
		delete m_camera_storage.Erase(index);
	}

	void _Assets::deleteAllTextures()
	{
		auto& textures = m_baseTexture_storage.GetAll(m_creationType);
		for (auto& texture : textures)
			delete texture.second;
		m_baseTexture_storage.EraseAll(m_creationType);
		m_texture2D_storage.EraseAll(m_creationType);
		m_cubemap_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllTexture2D()
	{
		auto& textures = m_texture2D_storage.GetAll(m_creationType);
		for (auto& texture : textures)
		{
			m_baseTexture_storage.Erase(texture.first);
			delete texture.second;
		}
		m_texture2D_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllCubemap()
	{
		auto& textures = m_cubemap_storage.GetAll(m_creationType);
		for (auto& texture : textures)
		{
			m_baseTexture_storage.Erase(texture.first);
			delete texture.second;
		}
		m_cubemap_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllFile()
	{
		auto& files = m_file_storage.GetAll(m_creationType);
		for (auto& file : files)
			delete file.second;

		m_file_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllFramebufferObject()
	{
		auto& fbos = m_framebufferObject_storage.GetAll(m_creationType);
		for (auto& fbo : fbos)
			delete fbo.second;

		m_framebufferObject_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllRenderTexture()
	{
		auto& rts = m_renderTexture_storage.GetAll(m_creationType);
		for (auto& rt : rts)
			delete rt.second;

		m_renderTexture_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllRenderTextureDepth()
	{
		auto& rts = m_renderTextureDepth_storage.GetAll(m_creationType);
		for (auto& rt : rts)
			delete rt.second;

		m_renderTextureDepth_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllRenderBuffer()
	{
		auto& rbs = m_renderBuffer_storage.GetAll(m_creationType);
		for (auto& rb : rbs)
			delete rb.second;

		m_renderBuffer_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllRenderBufferDepth()
	{
		auto& rbs = m_renderBufferDepth_storage.GetAll(m_creationType);
		for (auto& rb : rbs)
			delete rb.second;

		m_renderBufferDepth_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllMesh()
	{
		auto& Meshes = m_mesh_storage.GetAll(m_creationType);
		for (auto& mesh : Meshes)
			delete mesh.second;

		m_mesh_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllLineMesh3D()
	{
		auto& Meshes = m_lineMesh3D_storage.GetAll(m_creationType);
		for (auto& mesh : Meshes)
			delete mesh.second;

		m_lineMesh3D_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllLineMesh2D()
	{
		auto& Meshes = m_lineMesh2D_storage.GetAll(m_creationType);
		for (auto& mesh : Meshes)
			delete mesh.second;

		m_lineMesh2D_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllShaderProgram()
	{
		auto& shaderPrograms = m_shaderProgram_storage.GetAll(m_creationType);
		for (auto& sp : shaderPrograms)
			delete sp.second;

		m_shaderProgram_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllShaderMaterial()
	{
		auto& shaderMaterials = m_shaderMaterial_storage.GetAll(m_creationType);
		for (auto& sm : shaderMaterials)
			delete sm.second;

		m_shaderMaterial_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllMaterial()
	{
		auto& materials = m_material_storage.GetAll(m_creationType);
		for (auto& mat : materials)
			delete mat.second;

		m_material_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllSceneNode()
	{
		auto& nodes = m_sceneNode_storage.GetAll(m_creationType);
		for (auto& node : nodes)
		{
			delete node.second;
		}
		m_sceneNode_storage.EraseAll(m_creationType);
		m_entity_storage.EraseAll(m_creationType);
		m_camera_storage.EraseAll(m_creationType);
	}
	void _Assets::deleteAllEntity()
	{
		auto& entities = m_entity_storage.GetAll(m_creationType);
		for (auto& ent : entities)
		{
			// Remove Node
			m_sceneNode_storage.Erase(ent.first);
		
			delete ent.second;
		}
		
		m_material_storage.EraseAll(m_creationType);
		
		// RenderManager re-sort
		RenderManager.dirtyEntitySequence();
	}
	void _Assets::deleteAllCamera()
	{
		auto& cameras = m_camera_storage.GetAll(m_creationType);
		for (auto& cam : cameras)
		{
			// Remove Node
			m_sceneNode_storage.Erase(cam.first);
		
			delete cam.second;
		}
		
		m_camera_storage.EraseAll(m_creationType);
	}

	// Creation
	TextureIndex _Assets::loadTexture2D(
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
	TextureIndex _Assets::createTexture2D(
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
	TextureIndex _Assets::createTexture2D(
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
	TextureIndex _Assets::createTexture2D(
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
	TextureIndex _Assets::createTexture2D(
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
	TextureIndex _Assets::loadCubemap(
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
	void _Assets::loadFile(
		const std::string& name,
		const std::string& filepath
	)
	{
		// Create New Data
		File* _file = new File(filepath);
		// Store Data and Return index
		m_file_storage.Add(stringUtil::toLowerCopy(name), _file, m_creationType);
	}
	FramebufferObjectIndex _Assets::createFramebuffer(
		const std::string& name
	)
	{
		// Create New Data
		FramebufferObject* _fbo = new FramebufferObject(name);
		// Store Data and Return index
		return m_framebufferObject_storage.Add(_fbo, m_creationType);
	}
	RenderTextureIndex _Assets::createRenderTexture(
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
	RenderTextureDepthIndex _Assets::createRenderTextureDepth(
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
	RenderBufferIndex _Assets::createRenderBuffer(
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
	RenderBufferDepthIndex _Assets::createRenderBufferDepth(
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
	MeshIndex _Assets::createMesh(DrawType type)
	{
		// Create New Data
		Mesh* _mesh = new Mesh(type);
		// Store Data and Return index
		return m_mesh_storage.Add(_mesh, m_creationType);
	}
	LineMesh3DIndex _Assets::createLineMesh3D(DrawType type)
	{
		// Create New Data
		LineMesh3D* _mesh = new LineMesh3D(type);
		// Store Data and Return index
		return m_lineMesh3D_storage.Add(_mesh, m_creationType);
	}
	LineMesh3DIndex _Assets::createLineMesh2D(DrawType type)
	{
		// Create New Data
		LineMesh2D* _mesh = new LineMesh2D(type);
		// Store Data and Return index
		return m_lineMesh2D_storage.Add(_mesh, m_creationType);
	}

	ShaderIndex _Assets::createShader(const std::string& name, const std::string& source, ShaderType type)
	{
		// Create New Data
		Shader* _shader = new Shader(name, source, type);
		// Store Data and Return index
		return m_shader_storage.Add(_shader, m_creationType);
	}
	ShaderProgramIndex _Assets::createShaderProgram(const std::string& name, const std::vector<ShaderIndex>& _shaders, std::vector<std::pair<std::string, TextureLevel>> _textureLevels)
	{
		// Create New Data
		ShaderProgram* _program = new ShaderProgram(name, _shaders, _textureLevels);
		// Store Data and Return index
		return m_shaderProgram_storage.Add(_program, m_creationType);
	}

	MaterialIndex _Assets::createMaterial(const std::string& name)
	{
		// Create New Data
		Material* material = new Material(name);
		// Store Data and Return index
		MaterialIndex materialIndex = m_material_storage.Add(material, m_creationType);

		// Return Set info
		return materialIndex;
	}

	EntityIndex _Assets::createEntity(const std::string& name)
	{
		// Create New Data
		Entity* object = new Entity(name);
		
		// Store Data
		SceneNodeIndex nodeIndex = m_sceneNode_storage.Add(dynamic_cast<SceneNode*>(object), m_creationType);
		m_entity_storage.AddCustom(object, m_creationType, nodeIndex);
		object->m_uniqueID	= nodeIndex;

		// Special
		object->m_colorID	= Util::Conversion::uint_to_color4(object->m_uniqueID);
		
		// Rendermanager must re-sort Entities
		RenderManager.dirtyEntitySequence();
		
		// Return index
		return nodeIndex;
	}
	CameraIndex _Assets::createCamera(const std::string& name, float znear, float zfar)
	{
		// Create New Data
		Camera* object = new Camera(name, znear, zfar);
		object->m_transform.m_rotationOrder = EulerRotationOrder::YXZ;
		
		// Store Data
		SceneNodeIndex nodeIndex = m_sceneNode_storage.Add(dynamic_cast<SceneNode*>(object), m_creationType);
		m_camera_storage.AddCustom(object, m_creationType, nodeIndex);
		object->m_uniqueID = nodeIndex;

		// Return index
		return nodeIndex;
	}
	
	ShaderMaterialIndex _Assets::createShaderMaterial(const std::string& filePath)
	{
		// Create New Data
		ShaderMaterial* object = new ShaderMaterial(filePath, m_creationType == AssetType::GLOBAL);
		// Store Data and Return index
		return m_shaderMaterial_storage.Add(object, m_creationType);
	}
}