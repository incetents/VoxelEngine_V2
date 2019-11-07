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
	IDStorage<Texture2D>		  Assets::m_texture2D_storage;
	IDStorage<Cubemap>			  Assets::m_cubemap_storage;
	NamedStorage<File>			  Assets::m_file_storage;
	IDStorage<FramebufferObject>  Assets::m_framebufferObject_storage;
	IDStorage<RenderTexture>	  Assets::m_renderTexture_storage;
	IDStorage<RenderTextureDepth> Assets::m_renderTextureDepth_storage;
	IDStorage<RenderBuffer>		  Assets::m_renderBuffer_storage;
	IDStorage<RenderBufferDepth>  Assets::m_renderBufferDepth_storage;
	IDStorage<Mesh>				  Assets::m_mesh_storage;
	IDStorage<_Shader>			  Assets::m_shader_storage;
	IDStorage<_ShaderProgram>	  Assets::m_shaderProgram_storage;
	IDStorage<_Material>		  Assets::m_material_storage;
	IDStorage<Entity>			  Assets::m_entity_storage;
	IDStorage<Camera>			  Assets::m_camera_storage;

	void Assets::DestroyAndEraseAll()
	{
		// Delete All Assets
		auto& textures = getAllTexture2D();
		for (auto& texture : textures)
			delete texture.second;

		auto& cubemaps = getAllCubemap();
		for (auto& cubemap : cubemaps)
			delete cubemap.second;

		auto& files = getAllFiles();
		for (auto& file : files)
			delete file.second;

		auto& fbos = getAllFramebufferObject();
		for (auto& fbo : fbos)
			delete fbo.second;

		auto& renderTexture = getAllRenderTexture();
		for (auto& rt : renderTexture)
			delete rt.second;

		auto& renderTextureDepth = getAllRenderTextureDepth();
		for (auto& rtd : renderTextureDepth)
			delete rtd.second;

		auto& renderBuffer = getAllRenderBuffer();
		for (auto& rb : renderBuffer)
			delete rb.second;

		auto& renderBufferDepth = getAllRenderBufferDepth();
		for (auto& rbd : renderBufferDepth)
			delete rbd.second;

		auto& meshes = getAllMesh();
		for (auto& mesh : meshes)
			delete mesh.second;

		auto& shaders = getAllShader();
		for (auto& shader : shaders)
			delete shader.second;

		auto& shaderPrograms = getAllShaderProgram();
		for (auto& sp : shaderPrograms)
			delete sp.second;

		auto& materials = getAllMaterial();
		for (auto& mat : materials)
			delete mat.second;

		auto& entities = getAllEntity();
		for (auto& entity : entities)
			delete entity.second;

		auto& cameras = getAllCamera();
		for (auto& cam : cameras)
			delete cam.second;

		// erase All Assets
		m_texture2D_storage.EraseAll(m_creationType);
		m_cubemap_storage.EraseAll(m_creationType);
		m_file_storage.EraseAll();
		m_framebufferObject_storage.EraseAll(m_creationType);
		m_renderTexture_storage.EraseAll(m_creationType);
		m_renderTextureDepth_storage.EraseAll(m_creationType);
		m_renderBuffer_storage.EraseAll(m_creationType);
		m_renderBufferDepth_storage.EraseAll(m_creationType);
		m_mesh_storage.EraseAll(m_creationType);
		m_shader_storage.EraseAll(m_creationType);
		m_shaderProgram_storage.EraseAll(m_creationType);
		m_material_storage.EraseAll(m_creationType);
		m_entity_storage.EraseAll(m_creationType);
		m_camera_storage.EraseAll(m_creationType);
	}
	// Deletion
	void Assets::deleteTexture2D(Texture2DIndex index)
	{
		delete m_texture2D_storage.Erase(index);
	}
	void Assets::deleteCubemap(CubemapIndex index)
	{
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
	void Assets::deleteShader(ShaderIndex index)
	{
		delete m_shader_storage.Erase(index);
	}
	void Assets::deleteShaderProgram(ShaderProgramIndex index)
	{
		delete m_shaderProgram_storage.Erase(index);
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

	// Creation
	Texture2DIndex Assets::loadTexture2D(
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
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture, m_creationType);
	}
	Texture2DIndex Assets::createTexture2D(
		std::vector<Color3F> pixels, uint32_t width,
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
		Texture2D* _texture = new Texture2D(pixels, width, UseMipMapping, WrapMode, FilterMode, FormatType, ChannelType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture, m_creationType);
	}
	Texture2DIndex Assets::createTexture2D(
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
		// Store Data and Return index
		return m_texture2D_storage.Add(_texture, m_creationType);
	}
	CubemapIndex Assets::loadCubemap(
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
		// Create New DAta
		Cubemap* _cubemap = new Cubemap(filePath1, filePath2, filePath3, filePath4, filePath5, filePath6, InvertY, UseMipMapping, WrapMode, FilterMode, FormatType, PixelType, AnisotropicMode);
		// Store Data and Return index
		return m_cubemap_storage.Add(_cubemap, m_creationType);
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
	FramebufferObjectIndex Assets::createFramebuffer()
	{
		// Create New Data
		FramebufferObject* _fbo = new FramebufferObject();
		// Store Data and Return index
		return m_framebufferObject_storage.Add(_fbo, m_creationType);
	}
	RenderTextureIndex Assets::createRenderTexture(
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType,
		bool MipMapping
	)
	{
		// Create New Data
		RenderTexture* _texture = new RenderTexture(Width, Height, FormatType, PixelType, MipMapping);
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
		int Width,
		int Height,
		TextureFormat FormatType,
		TexturePixelType PixelType
	)
	{
		// Create New Data
		RenderBuffer* _buffer = new RenderBuffer(Width, Height, FormatType, PixelType);
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

	ShaderIndex Assets::createShader(const std::string& name, const std::string& sourceCode, ShaderType type)
	{
		// Create New Data
		_Shader* _shader = new _Shader(name, sourceCode, type);
		// Store Data and Return index
		return m_shader_storage.Add(_shader, m_creationType);
	}
	ShaderProgramIndex Assets::createShaderProgram(const std::string& name, const std::vector<_Shader*>& _shaders)
	{
		// Create New Data
		_ShaderProgram* _program = new _ShaderProgram(name, _shaders);
		// Store Data and Return index
		return m_shaderProgram_storage.Add(_program, m_creationType);
	}
	ShaderProgramIndex Assets::createShaderProgram(const std::string& name, std::initializer_list<_Shader*> _shaders)
	{
		// Create New Data
		_ShaderProgram* _program = new _ShaderProgram(name, _shaders);
		// Store Data and Return index
		return m_shaderProgram_storage.Add(_program, m_creationType);
	}

	MaterialIndex Assets::createMaterial(const std::string& name)
	{
		// Create New Data
		_Material* material = new _Material(name);
		// Store Data and Return index
		MaterialIndex materialIndex = m_material_storage.Add(material, m_creationType);

		// Return Set info
		return materialIndex;
	}

	EntityIndex Assets::createEntity(const std::string& name)
	{
		// Create New Data
		Entity* object = new Entity(name);
		object->m_uniqueID = 0;
		object->m_colorID = Util::Conversion::uint_to_color4(object->m_uniqueID);
		// Store Data and Return index
		return m_entity_storage.Add(object, m_creationType);
	}
	CameraIndex Assets::createCamera(const std::string& name, float znear, float zfar)
	{
		// Create New Data
		Camera* object = new Camera(name, znear, zfar);
		// Store Data and Return index
		return m_camera_storage.Add(object, m_creationType);
	}
	

	const char* SECTION_NAME = "#Name";
	const char* SECTION_INCLUDE = "#Include";
	const char* SECTION_PROPERTIES = "#Properties";
	const char* SECTION_ATTRIBUTE = "#Attributes";
	const char* SECTION_VERTEX = "#Vertex";
	const char* SECTION_GEOMETRY = "#Geometry";
	const char* SECTION_FRAGMENT = "#Fragment";

	void MaterialMaker(const std::string& filepath)
	{
		std::string file = FileIO::readFile(filepath);
		if (file.empty())
			return;

		std::string VertexShaderCode;
		std::string GeometryShaderCode;
		std::string FragmentShaderCode;

		struct OUTPUT
		{
			bool active;
			std::string version = "#version 420 core";
			std::string include;
			std::string input;
			std::string output;
			std::string behaviour;
		};

		OUTPUT output_vertex;
		OUTPUT output_geometry;
		OUTPUT output_fragment;

		struct SECTION
		{
			std::size_t name;
			std::size_t include;
			std::size_t properties;
			std::size_t attributes;
			std::size_t vertex;
			std::size_t geometry;
			std::size_t fragment;
		} locations;

		locations.name = file.find(SECTION_NAME);
		locations.include = file.find(SECTION_INCLUDE);
		locations.properties = file.find(SECTION_PROPERTIES);
		locations.attributes = file.find(SECTION_ATTRIBUTE);
		locations.vertex = file.find(SECTION_VERTEX);
		locations.geometry = file.find(SECTION_GEOMETRY);
		locations.fragment = file.find(SECTION_FRAGMENT);

		// Quick active state check
		output_vertex.active = (locations.vertex != std::string::npos);
		output_geometry.active = (locations.geometry != std::string::npos);
		output_fragment.active = (locations.fragment != std::string::npos);

		/*
		// Name
		if (locations.name != std::string::npos)
		{
			Name = stringUtil::extractSection(file, '{', '}', locations.name);
			stringUtil::trim(Name);
		}
		*/

		// Include
		if (locations.include != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.include);
			std::vector<std::string> includes = stringUtil::splitStr(section, ',');
			for (auto& include : includes)
			{
				stringUtil::trim(include);
				File* _fileStorage = Assets::getFile(include);
				if (_fileStorage)
				{
					std::string file = _fileStorage->file + '\n';

					if (output_vertex.active)
						output_vertex.include += file;

					if (output_geometry.active)
						output_geometry.include += file;

					if (output_fragment.active)
						output_fragment.include += file;
				}
			}
		}

		// Properties
		if (locations.properties != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.properties);

			if (output_vertex.active)
				output_vertex.include += section;

			if (output_geometry.active)
				output_geometry.include += section;

			if (output_fragment.active)
				output_fragment.include += section;
		}

		// Attributes
		if (locations.attributes != std::string::npos)
		{
			output_vertex.input = "// Attributes\n";

			// Acquire all
			std::string section = stringUtil::extractSection(file, '{', '}', locations.attributes);
			std::vector<std::string> lines = stringUtil::splitStr(section, '\n');
			for (auto& line : lines)
			{
				line = stringUtil::stripComments(line);
				if (line.empty())
					continue;

				// [ First Section : Second Section ]
				std::vector<std::string> property = stringUtil::splitStr(line, ':');
				if (property.size() != 2)
					continue;

				stringUtil::trim(property[0]);
				stringUtil::trim(property[1]);

				// Add attribute information
				output_vertex.input += std::string("layout (location = " + property[1] + ") in " + property[0] + ";\n");
			}
		}
		// Vertex
		if (output_vertex.active)
		{
			output_vertex.behaviour = stringUtil::extractSection(file, '{', '}', locations.vertex);

			VertexShaderCode =
				output_vertex.version + '\n' +
				output_vertex.include + '\n' +
				output_vertex.input + '\n' +
				output_vertex.output + '\n' +
				output_vertex.behaviour;
		}
		// Geometry
		if (output_geometry.active)
		{
			output_geometry.behaviour = stringUtil::extractSection(file, '{', '}', locations.geometry);

			GeometryShaderCode =
				output_geometry.version + '\n' +
				output_geometry.include + '\n' +
				output_geometry.input + '\n' +
				output_geometry.output + '\n' +
				output_geometry.behaviour;
		}
		// Fragment
		if (output_fragment.active)
		{
			output_fragment.behaviour = stringUtil::extractSection(file, '{', '}', locations.fragment);

			FragmentShaderCode =
				output_fragment.version + '\n' +
				output_fragment.include + '\n' +
				output_fragment.input + '\n' +
				output_fragment.output + '\n' +
				output_fragment.behaviour;
		}

		// Create Shaders
		std::vector<_Shader*> shaders;

		//	if (!VertexShaderCode.empty())
		//		shaders.push_back(new _Shader("", VertexShaderCode, ShaderType::VERTEX));
		//	
		//	if (!GeometryShaderCode.empty())
		//		shaders.push_back(new _Shader("", GeometryShaderCode, ShaderType::GEOMETRY));
		//	
		//	if (!FragmentShaderCode.empty())
		//		shaders.push_back(new _Shader("", FragmentShaderCode, ShaderType::FRAGMENT));
	}
}