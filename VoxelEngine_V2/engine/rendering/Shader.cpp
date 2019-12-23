// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Shader.h"

#include "../math/Vector.h"

#include "../modules/Entity.h"

#include "../objects/Camera.h"

#include "../rendering/Mesh.h"
#include "../rendering/RenderManager.h"

#include "../utilities/Logger.h"
#include "../utilities/FileIO.h"
#include "../utilities/Asset.h"

#include "../window/window.h"

namespace Vxl
{
	std::map<ShaderID, Shader*> Shader::m_brokenShaders;
	std::map<ShaderProgramID, ShaderProgram*> ShaderProgram::m_brokenShaderPrograms;

	Shader::Shader(const std::string& name, const std::string& shaderCode, ShaderType type)
		: m_name(name), m_source(shaderCode), m_type(type)
	{
		// Create
		m_id = Graphics::Shader::Create(m_type);
		if (m_id == -1)
		{
			m_compiled = false;
			return;
		}

		// Fix includes
		m_source = FileIO::addInclude(m_source);

		// Lines for source
		m_sourceLineCount = (uint32_t)stringUtil::countChar(m_source, '\n') + 1;
		for (uint32_t i = 1; i <= m_sourceLineCount; i++)
			m_sourceLines += std::to_string(i) + '\n';

		// Convert file to const char
		const char* Source = static_cast<const char *>(m_source.c_str());

		// Attach Shader and Compile
		m_compiled = Graphics::Shader::Compile(m_id, m_type, Source);

		// Error
		if (!m_compiled)
		{
			m_errorMessage = "Failed to Compile Shader\n";
			m_errorMessage += "Name: " + m_name + "\n";

			switch (m_type)
			{
			case ShaderType::VERTEX:
				m_errorMessage += ("Type: VERTEX\n");
				break;
			case ShaderType::FRAGMENT:
				m_errorMessage += ("Type: FRAGMENT\n");
				break;
			case ShaderType::GEOMETRY:
				m_errorMessage += ("Type: GEOMETRY\n");
				break;
			case ShaderType::TESSELATION_CONTROL:
				m_errorMessage += ("Type: TESSELATION CONTROL\n");
				break;
			case ShaderType::TESSELATION_EVALUATION:
				m_errorMessage += ("Type: TESSELATION EVALUATION\n");
				break;
			case ShaderType::COMPUTE:
				m_errorMessage += ("Type: COMPUTE SHADER\n");
				break;
			default:
				m_errorMessage += ("Type: UNKNOWN SHADER\n");
				break;
			}
			m_errorMessage += "Error:\n";
			m_errorMessage += Graphics::Shader::GetError(m_id);
			m_errorMessage += "~~~~~~~~~~\n";
			m_errorMessage.shrink_to_fit();

			m_brokenShaders[m_id] = this;

		}
	}
	Shader::~Shader()
	{
		m_brokenShaders.erase(m_id);

		if (m_id != -1)
			Graphics::Shader::Delete(m_id);
	}
	void Shader::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::SHADER, m_id, "Shader_" + name);
	}

	void ShaderProgram::setupCommonUniform(const std::string& name, std::optional<Graphics::Uniform>& uniform)
	{
		auto uniformIterator = m_uniforms.find(name);
		if (uniformIterator != m_uniforms.end())
			uniform = uniformIterator->second;
		else
			uniform = std::nullopt;
	}

	ShaderProgram::ShaderProgram(const std::string& name, const std::vector<ShaderIndex>& _shaders)
		: m_name(name), m_shaders(_shaders)
	{
		m_id = Graphics::ShaderProgram::Create();
		if (m_id == -1)
		{
			m_linked = false;
			return;
		}

		uint32_t shaderCount = (uint32_t)m_shaders.size();

		// Auto fail if any shaders aren't compiled
		for (uint32_t i = 0; i < shaderCount; i++)
		{
			Shader* _shader = Assets.getShader(m_shaders[i]);

			if (!_shader->isCompiled())
			{
				m_linked = false;
				return;
			}
		}

		// Attach Shaders to Shader Program
		for (uint32_t i = 0; i < shaderCount; i++)
		{
			Shader* _shader = Assets.getShader(m_shaders[i]);

			Graphics::ShaderProgram::AttachShader(m_id, _shader->getID());
		}

		// Link
		m_linked = Graphics::ShaderProgram::Link(m_id);

#if _DEBUG
		// Validation
		m_linked &= Graphics::ShaderProgram::Validate(m_id);
#endif

		if (m_linked)
		{
			// attributes //
			m_attributes = Graphics::ShaderProgram::AcquireAttributes(m_id);
			
			// uniforms //
			m_uniforms = Graphics::ShaderProgram::AcquireUniforms(m_id);

			// uniform storage // (stores intermediate values)
			for (const auto& uniform : m_uniforms)
			{
				// Ignore VLX_ uniforms
				if (uniform.first.substr(0, 4).compare("VXL_") != 0 && uniform.second.isData)
				{
					// Check UniformType
					UniformType utype = uniform.second.uType;
					switch (utype)
					{
					case UniformType::FLOAT:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, 0.f), false };
						break;
					case UniformType::FLOAT_VEC2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector2(0, 0)), false };
						break;
					case UniformType::FLOAT_VEC3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector3(0, 0, 0)), false };
						break;
					case UniformType::FLOAT_VEC4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector4(0, 0, 0, 0)), false };
						break;

					case UniformType::DOUBLE:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, 0.0), false };
						break;
					case UniformType::DOUBLE_VEC2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector2d(0, 0)), false };
						break;
					case UniformType::DOUBLE_VEC3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector3d(0, 0, 0)), false };
						break;
					case UniformType::DOUBLE_VEC4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector4d(0, 0, 0, 0)), false };
						break;

					case UniformType::INT:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, 0), false };;
						break;
					case UniformType::INT_VEC2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector2i(0, 0)), false };
						break;
					case UniformType::INT_VEC3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector3i(0, 0, 0)), false };
						break;
					case UniformType::INT_VEC4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector4i(0, 0, 0, 0)), false };
						break;

					case UniformType::UNSIGNED_INT:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, 0u), false };
						break;
					case UniformType::UNSIGNED_INT_VEC2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector2ui(0u, 0u)), false };
						break;
					case UniformType::UNSIGNED_INT_VEC3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector3ui(0u, 0u, 0u)), false };
						break;
					case UniformType::UNSIGNED_INT_VEC4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector4ui(0u, 0u, 0u, 0u)), false };
						break;

					case UniformType::BOOL:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, false), false };
						break;
					case UniformType::BOOL_VEC2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector2b(false, false)), false };
						break;
					case UniformType::BOOL_VEC3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector3b(false, false, false)), false };
						break;
					case UniformType::BOOL_VEC4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Vector4b(false, false, false, false)), false };
						break;

					case UniformType::FLOAT_MAT2:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Matrix2x2::Identity), false };
						break;
					case UniformType::FLOAT_MAT3:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Matrix3x3::Identity), false };
						break;
					case UniformType::FLOAT_MAT4:
						m_uniformStorage[uniform.first] = UniformStorage{ RawData(utype, Matrix4x4::Identity), false };
						break;

					default:
						VXL_ERROR("Uniform Type not supported for Uniform Storage system");
					}
				}
			}

			// uniform blocks
			m_uniformBlocks = Graphics::ShaderProgram::AcquireUniformBlocks(m_id);

			// subroutines
			std::vector<ShaderType> types;
			types.reserve(shaderCount);
			for (uint32_t shaderIndex : m_shaders)
			{
				Shader* shader = Assets.getShader(shaderIndex);
				types.push_back(shader->getType());
			}

			m_subroutines = Graphics::ShaderProgram::AcquireUniformSubroutines(m_id, types);

			// Setup common uniforms
			setupCommonUniform("VXL_useModel", m_uniform_useModel);
			setupCommonUniform("VXL_model", m_uniform_model);
			setupCommonUniform("VXL_mvp", m_uniform_mvp);
			setupCommonUniform("VXL_normalMatrix", m_uniform_normalMatrix);
			setupCommonUniform("VXL_useInstancing", m_uniform_useInstancing);
			setupCommonUniform("VXL_useTexture", m_uniform_useTexture);
			setupCommonUniform("VXL_color", m_uniform_color);
			setupCommonUniform("VXL_tint", m_uniform_tint);
			setupCommonUniform("VXL_alpha", m_uniform_alpha);
			setupCommonUniform("VXL_output", m_uniform_output);
			setupCommonUniform("VXL_colorID", m_uniform_colorID);
		}
		else
		{
			m_errorMessage = Graphics::ShaderProgram::GetError(m_id);

			Logger.error(m_errorMessage);

			m_brokenShaderPrograms[m_id] = this;
		}

		// Detach Shaders from Shader Program
		for (unsigned int i = 0; i < shaderCount; i++)
		{
			Shader* _Shader = Assets.getShader(m_shaders[i]);
			Graphics::ShaderProgram::DetachShader(m_id, _Shader->getID());
		}

	}
	ShaderProgram::~ShaderProgram()
	{
		m_brokenShaderPrograms.erase(m_id);

		Graphics::ShaderProgram::Delete(m_id);
	}

	void ShaderProgram::bind() const
	{
		if (m_linked)
		{
			Graphics::ShaderProgram::Enable(m_id);

			// bind subroutines
			if (!m_subroutines.empty())
			{
				for (const auto& s : m_subroutines)
					s.second.bind();
			}
		}
	}
	void ShaderProgram::unbind()
	{
		Graphics::ShaderProgram::Disable();
	}
	void ShaderProgram::setGLName(const std::string& name)
	{
		Graphics::SetGLName(ObjectType::PROGRAM, m_id, "Program_" + name);
	}

	// Binding Common Uniforms [VXL_]
	void ShaderProgram::bindCommonUniforms(EntityIndex _entity)
	{
		if (_entity == -1)
			return;

		Entity* entity = Assets.getEntity(_entity);
		if (!entity)
			return;

		Material* material = Assets.getMaterial(entity->m_material);
		if (!material)
			return;

		// ~ Model (and useModel) ~ //
		if (m_uniform_useModel.has_value())
		{
			// Transform override
			m_uniform_useModel.value().send(entity->m_useTransform);

			if (entity->m_useTransform)
			{
				if (m_uniform_model.has_value())
					m_uniform_model.value().sendMatrix(entity->m_transform.getModel(), true);
			}
		}

		// ~ MVP Matrix ~ //
		if (m_uniform_mvp.has_value() && entity->m_useTransform)
		{
			Camera* camera = Assets.getCamera(RenderManager.m_mainCamera);
			if (camera)
				m_uniform_mvp.value().sendMatrix(camera->getViewProjection() * entity->m_transform.getModel(), true);
		}

		// ~ Normal Matrix ~ //
		if (m_uniform_normalMatrix.has_value() && entity->m_useTransform)
		{
			m_uniform_normalMatrix.value().sendMatrix(entity->m_transform.getNormalMatrix(), true);
		}

		// ~ Instancing ~ //
		if (m_uniform_useInstancing.has_value())
		{
			Mesh* mesh = Assets.getMesh(entity->getMesh());

			m_uniform_useInstancing.value().send(mesh && mesh->m_instances.getDrawCount() > 0);
		}

		// ~ Texture ~ //
		if (m_uniform_useTexture.has_value())
		{
			if (material->m_sharedTextures)
				m_uniform_useTexture.value().send(material->m_textures.find(TextureLevel::LEVEL0) != material->m_textures.end());
			else if (entity->m_useTextures)
				m_uniform_useTexture.value().send(entity->m_textures.find(TextureLevel::LEVEL0) != entity->m_textures.end());
			else
				m_uniform_useTexture.value().send(false);
		}

		// ~ Colors ~ //
		if (m_uniform_color.has_value())
		{
			m_uniform_color.value().send(entity->m_Color);
		}
		// ~ Tint ~ //
		if (m_uniform_tint.has_value())
		{
			m_uniform_tint.value().send(entity->m_Tint);
		}

		// ~ Alpha ~ //
		if (m_uniform_alpha.has_value())
		{
			if (material->m_renderMode == MaterialRenderMode::Transparent)
				m_uniform_alpha.value().send(entity->m_alpha);
			else
				m_uniform_alpha.value().send(1.0f);
		}

		// ~ ColorID ~ //
		if (m_uniform_colorID.has_value())
		{
			m_uniform_colorID.value().send(entity->m_colorID);
		}
	}

	// Binding Custom Uniforms [Non VXL_]
	void ShaderProgram::bindCustomUniforms()
	{
		for (auto& uniformStorage : m_uniformStorage)
		{
			// Don't send uniforms that haven't been modified
			if (!uniformStorage.second.m_dirty)
				continue;

			// Dirty flag
			uniformStorage.second.m_dirty = false;

			// Acquire Uniform
			Graphics::Uniform& uniform = m_uniforms[uniformStorage.first];
			// Acquire Data
			switch (uniformStorage.second.m_data.type)
			{
			case UniformType::FLOAT:
				uniform.send(*uniformStorage.second.m_data.getData<float>());
				break;
			case UniformType::FLOAT_VEC2:
				uniform.send(*uniformStorage.second.m_data.getData<Vector2>());
				break;
			case UniformType::FLOAT_VEC3:
				uniform.send(*uniformStorage.second.m_data.getData<Vector3>());
				break;
			case UniformType::FLOAT_VEC4:
				uniform.send(*uniformStorage.second.m_data.getData<Vector4>());
				break;

			case UniformType::DOUBLE:
				uniform.send(*uniformStorage.second.m_data.getData<double>());
				break;
			case UniformType::DOUBLE_VEC2:
				uniform.send(*uniformStorage.second.m_data.getData<Vector2d>());
				break;
			case UniformType::DOUBLE_VEC3:
				uniform.send(*uniformStorage.second.m_data.getData<Vector3d>());
				break;
			case UniformType::DOUBLE_VEC4:
				uniform.send(*uniformStorage.second.m_data.getData<Vector4d>());
				break;

			case UniformType::INT:
				uniform.send(*uniformStorage.second.m_data.getData<int>());
				break;
			case UniformType::INT_VEC2:
				uniform.send(*uniformStorage.second.m_data.getData<Vector2i>());
				break;
			case UniformType::INT_VEC3:
				uniform.send(*uniformStorage.second.m_data.getData<Vector3i>());
				break;
			case UniformType::INT_VEC4:
				uniform.send(*uniformStorage.second.m_data.getData<Vector4i>());
				break;

			case UniformType::UNSIGNED_INT:
				uniform.send(*uniformStorage.second.m_data.getData<uint32_t>());
				break;
			case UniformType::UNSIGNED_INT_VEC2:
				uniform.send(*uniformStorage.second.m_data.getData<Vector2ui>());
				break;
			case UniformType::UNSIGNED_INT_VEC3:
				uniform.send(*uniformStorage.second.m_data.getData<Vector3ui>());
				break;
			case UniformType::UNSIGNED_INT_VEC4:
				uniform.send(*uniformStorage.second.m_data.getData<Vector4ui>());
				break;

			case UniformType::BOOL:
				uniform.send(*uniformStorage.second.m_data.getData<bool>());
				break;
			case UniformType::BOOL_VEC2:
				uniform.send(*uniformStorage.second.m_data.getData<Vector2b>());
				break;
			case UniformType::BOOL_VEC3:
				uniform.send(*uniformStorage.second.m_data.getData<Vector3b>());
				break;
			case UniformType::BOOL_VEC4:
				uniform.send(*uniformStorage.second.m_data.getData<Vector4b>());
				break;

			case UniformType::FLOAT_MAT2:
				uniform.sendMatrix(*uniformStorage.second.m_data.getData<Matrix2x2>(), uniformStorage.second.m_transpose);
				break;
			case UniformType::FLOAT_MAT3:
				uniform.sendMatrix(*uniformStorage.second.m_data.getData<Matrix3x3>(), uniformStorage.second.m_transpose);
				break;
			case UniformType::FLOAT_MAT4:
				uniform.sendMatrix(*uniformStorage.second.m_data.getData<Matrix4x4>(), uniformStorage.second.m_transpose);
				break;

			default:
				VXL_ERROR("Trying to send custom uniform with non-supported type");
			}
		}
	}

	const char* SECTION_NAME = "#Name";
	const char* SECTION_INCLUDE = "#Include";
	const char* SECTION_ATTRIBUTE = "#Attributes";
	const char* SECTION_LINK = "#Link";
	const char* SECTION_RENDERTARGETS = "#RenderTargets";
	const char* SECTION_SAMPLERS = "#Samplers";
	const char* SECTION_PROPERTIES = "#Properties";
	const char* SECTION_VERTEX = "#Vertex";
	const char* SECTION_GEOMETRY = "#Geometry";
	const char* SECTION_FRAGMENT = "#Fragment";
	const char* SECTION_VERTEX_DEFINES = "#DefinesVertex";
	const char* SECTION_GEOMETRY_DEFINES = "#DefinesGeometry";
	const char* SECTION_FRAGMENT_DEFINES = "#DefinesFragment";

	//const std::string GLSL_VERSION = "#version 420 core";

	ShaderMaterial::ShaderMaterial(const std::string& filePath, bool GlobalAsset)
		: m_filePath(filePath), m_isGlobal(GlobalAsset)
	{
		reload();
	}

	void ShaderMaterial::reload()
	{
		// Delete existing ShaderPrograms
		Assets.deleteShaderProgram(m_coreProgram);
		Assets.deleteShaderProgram(m_colorIDProgram);

		m_targetLevels.clear();
		m_coreProgram = -1;
		m_colorIDProgram = -1;

		std::string file = FileIO::readFile(m_filePath);
		if (file.empty())
			return;

		std::string CORE_VertexShaderCode;
		std::string CORE_GeometryShaderCode;
		std::string CORE_FragmentShaderCode;
		std::string COLORID_FragmentShaderCode;

		struct OUTPUT
		{
			bool active;
			std::string o_defines;
			std::string o_include;
			std::string o_attributes;
			std::string o_link;
			std::string o_rendertargets;
			std::string o_samplers;
			std::string o_properties;
			std::string o_main;
		};

		OUTPUT output_vertex;
		OUTPUT output_geometry;
		OUTPUT output_fragment;

		struct SECTION
		{
			std::size_t name;
			std::size_t include;
			std::size_t attributes;
			std::size_t link;
			std::size_t rendertargets;
			std::size_t samplers;
			std::size_t properties;
			std::size_t vertex_defines;
			std::size_t geometry_defines;
			std::size_t fragment_defines;
			std::size_t vertex;
			std::size_t geometry;
			std::size_t fragment;
		} locations;

		locations.name = file.find(SECTION_NAME);
		locations.include = file.find(SECTION_INCLUDE);
		locations.attributes = file.find(SECTION_ATTRIBUTE);
		locations.link = file.find(SECTION_LINK);
		locations.rendertargets = file.find(SECTION_RENDERTARGETS);
		locations.samplers = file.find(SECTION_SAMPLERS);
		locations.properties = file.find(SECTION_PROPERTIES);
		locations.vertex = file.find(SECTION_VERTEX);
		locations.geometry = file.find(SECTION_GEOMETRY);
		locations.fragment = file.find(SECTION_FRAGMENT);
		locations.vertex_defines = file.find(SECTION_VERTEX_DEFINES);
		locations.geometry_defines = file.find(SECTION_GEOMETRY_DEFINES);
		locations.fragment_defines = file.find(SECTION_FRAGMENT_DEFINES);

		// Quick active state check
		output_vertex.active = (locations.vertex != std::string::npos);
		output_geometry.active = (locations.geometry != std::string::npos);
		output_fragment.active = (locations.fragment != std::string::npos);

		// Name
		std::string name;
		if (locations.name != std::string::npos)
		{
			name = stringUtil::extractSection(file, '{', '}', locations.name);
			stringUtil::trim(name);
		}
		else
		{
			name = stringUtil::extractNameFromPath(m_filePath);
		}

		// Defines
		if (locations.vertex_defines != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.vertex_defines);

			if (output_vertex.active)
				output_vertex.o_defines += "// Defines\n" + section + "\n\n";
		}
		if (locations.geometry_defines != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.geometry_defines);

			if (output_geometry.active)
				output_geometry.o_defines += "// Defines\n" + section + "\n\n";
		}
		if (locations.fragment_defines != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.fragment_defines);

			if (output_fragment.active)
				output_fragment.o_defines += "// Defines\n" + section + "\n\n";
		}

		// Include
		if (locations.include != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.include);
			std::vector<std::string> includes = stringUtil::splitStr(section, ',');
			for (auto& include : includes)
			{
				stringUtil::trim(include);
				File* _fileStorage = Assets.getFile(include);
				if (_fileStorage)
				{
					std::string file = _fileStorage->file + '\n';

					if (output_vertex.active)
						output_vertex.o_include += file + '\n';

					if (output_geometry.active)
						output_geometry.o_include += file + '\n';

					if (output_fragment.active)
						output_fragment.o_include += file + '\n';
				}
			}
		}

		// Attributes
		if (locations.attributes != std::string::npos && output_vertex.active)
		{
			output_vertex.o_attributes += "// Attributes\n";

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
				output_vertex.o_attributes += std::string("layout (location = " + property[1] + ") in " + property[0] + ";\n");
			}
			output_vertex.o_attributes += '\n';
		}

		// Link
		if (locations.link != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.link);

			if (output_vertex.active)
			{
				output_vertex.o_link += "// Output\n";
				output_vertex.o_link += "out LinkData\n{";
				output_vertex.o_link += section;
				output_vertex.o_link += "\n} vert_out;\n\n";
			}
			if (output_fragment.active)
			{
				output_fragment.o_link += "// Input\n";
				output_fragment.o_link += output_geometry.active ? "in LinkData_2\n{" : "in LinkData\n{";
				output_fragment.o_link += section;
				output_fragment.o_link += "\n} frag_in;\n\n";
			}
			if (output_geometry.active)
			{
				output_geometry.o_link += "// Input\n";
				output_geometry.o_link += "in LinkData\n{";
				output_geometry.o_link += section;
				output_geometry.o_link += "\n} geom_in[];\n\n";

				output_geometry.o_link += "// Output\n";
				output_geometry.o_link += "out LinkData_2\n{";
				output_geometry.o_link += section;
				output_geometry.o_link += "\n} geom_out;\n\n";
			}
		}

		// Render Targets
		if (locations.rendertargets != std::string::npos && output_fragment.active)
		{
			output_fragment.o_rendertargets += "// Render Targets\n";

			// Acquire all
			std::string section = stringUtil::extractSection(file, '{', '}', locations.rendertargets);
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
				output_fragment.o_rendertargets += std::string("layout (location = " + property[1] + ") out " + property[0] + ";\n");
			}
		}

		// Samplers
		std::vector<TextureLevel> targetLevels;
		if (locations.samplers != std::string::npos)
		{
			std::string section = stringUtil::extractSection(file, '{', '}', locations.samplers) + '\n';
			std::vector<std::string> lines = stringUtil::splitStr(section, '\n');

			std::string sampler_info = "// Samplers\n";

			for (auto& line : lines)
			{
				line = stringUtil::stripComments(line);
				if (line.empty())
					continue;

				std::vector<std::string> property = stringUtil::splitStr(line, ':');
				if (property.size() == 2)
				{
					stringUtil::trim(property[0]);
					stringUtil::trim(property[1]);

					if (stringUtil::isNumber(property[1]))
					{
						sampler_info += "layout (binding = " + property[1] + ") uniform " + property[0] + ";\n";

						targetLevels.push_back((TextureLevel)(std::stoi(property[1]) + 1));
					}
				}
			}

			if (output_vertex.active)
				output_vertex.o_samplers += sampler_info + '\n';

			if (output_geometry.active)
				output_geometry.o_samplers += sampler_info + '\n';

			if (output_fragment.active)
				output_fragment.o_samplers += sampler_info + '\n';
		}

		// Properties
		if (locations.properties != std::string::npos)
		{
			std::string section = "// Properties\n" + stringUtil::extractSection(file, '{', '}', locations.properties) + '\n';

			if (output_vertex.active)
				output_vertex.o_properties += section;

			if (output_geometry.active)
				output_geometry.o_properties += section;

			if (output_fragment.active)
				output_fragment.o_properties += section;
		}

		// Vertex
		if (output_vertex.active)
		{
			output_vertex.o_main =
				"// Main\n"
				"void main()\n"
				"{";
			output_vertex.o_main += stringUtil::extractSection(file, '{', '}', locations.vertex);
			output_vertex.o_main += "\n}";

			CORE_VertexShaderCode =
				Graphics::GLSL_Version + '\n' +
				output_vertex.o_defines + '\n' +
				output_vertex.o_include + '\n' +
				output_vertex.o_attributes + '\n' +
				output_vertex.o_link + '\n' +
				// no rendertargets
				output_vertex.o_samplers + '\n' +
				output_vertex.o_properties + '\n' +
				output_vertex.o_main;
		}
		// Geometry
		if (output_geometry.active)
		{
			output_geometry.o_main =
				"// Main\n"
				"void main()\n"
				"{";
			output_geometry.o_main += stringUtil::extractSection(file, '{', '}', locations.geometry);
			output_geometry.o_main += "\n}";

			CORE_GeometryShaderCode =
				Graphics::GLSL_Version + '\n' +
				output_geometry.o_defines + '\n' +
				output_geometry.o_include + '\n' +
				// no attributes
				output_geometry.o_link + '\n' +
				// no rendertargets
				output_geometry.o_samplers + '\n' +
				output_geometry.o_properties + '\n' +
				output_geometry.o_main;
		}
		// Fragment
		if (output_fragment.active)
		{
			output_fragment.o_main =
				"// Main\n"
				"void main()\n"
				"{";
			output_fragment.o_main += stringUtil::extractSection(file, '{', '}', locations.fragment);
			output_fragment.o_main += "\n}";

			CORE_FragmentShaderCode =
				Graphics::GLSL_Version + '\n' +
				output_fragment.o_defines + '\n' +
				output_fragment.o_include + '\n' +
				// no attributes
				output_fragment.o_link + '\n' +
				output_fragment.o_rendertargets + '\n' +
				output_fragment.o_samplers + '\n' +
				output_fragment.o_properties + '\n' +
				output_fragment.o_main;

			COLORID_FragmentShaderCode =
				Graphics::GLSL_Version + '\n' +
				output_fragment.o_defines + '\n' +
				output_fragment.o_include + '\n' +
				// no attributes
				output_fragment.o_link + '\n' +
				"layout (location =  0) out vec4 FragOutput ;\n" + // Custom RenderTarget
				output_fragment.o_samplers + '\n' +
				output_fragment.o_properties + '\n' +
				// Custom Main
				"// Main\n"
				"void main()\n"
				"{\n"
				"FragOutput = VXL_colorID;\n"
				"}";
		}

		// Create Shaders
		ShaderIndex CORE_VertexShader = -1;
		ShaderIndex CORE_GeometryShader = -1;
		ShaderIndex CORE_FragmentShader = -1;
		ShaderIndex COLORID_FragmentShader = -1;

		if (!CORE_VertexShaderCode.empty())
		{
			if (m_isGlobal)
				CORE_VertexShader = GlobalAssets.createShader(name + "_vert", CORE_VertexShaderCode, ShaderType::VERTEX);
			else
				CORE_VertexShader = SceneAssets.createShader(name + "_vert", CORE_VertexShaderCode, ShaderType::VERTEX);

		}

		if (!CORE_GeometryShaderCode.empty())
		{
			if (m_isGlobal)
				CORE_GeometryShader = GlobalAssets.createShader(name + "_geom", CORE_GeometryShaderCode, ShaderType::GEOMETRY);
			else
				CORE_GeometryShader = SceneAssets.createShader(name + "_geom", CORE_GeometryShaderCode, ShaderType::GEOMETRY);
		}

		if (!CORE_FragmentShaderCode.empty())
		{
			if (m_isGlobal)
			{
				CORE_FragmentShader = GlobalAssets.createShader(name + "_frag", CORE_FragmentShaderCode, ShaderType::FRAGMENT);
				COLORID_FragmentShader = GlobalAssets.createShader(name + "_colorID_frag", COLORID_FragmentShaderCode, ShaderType::FRAGMENT);
			}
			else
			{
				CORE_FragmentShader = SceneAssets.createShader(name + "_frag", CORE_FragmentShaderCode, ShaderType::FRAGMENT);
				COLORID_FragmentShader = SceneAssets.createShader(name + "_colorID_frag", COLORID_FragmentShaderCode, ShaderType::FRAGMENT);
			}
		}

		// Array of Shaders
		std::vector<ShaderIndex> CORE_Shaders;
		std::vector<ShaderIndex> COLORID_Shaders;
		// Vert
		if (CORE_VertexShader != -1)
		{
			CORE_Shaders.push_back(CORE_VertexShader);
			COLORID_Shaders.push_back(CORE_VertexShader);
		}
		// Geom
		if (CORE_GeometryShader != -1)
		{
			CORE_Shaders.push_back(CORE_GeometryShader);
			COLORID_Shaders.push_back(CORE_GeometryShader);
		}
		// Frag
		if (CORE_FragmentShader != -1)
		{
			CORE_Shaders.push_back(CORE_FragmentShader);
		}
		if (COLORID_FragmentShader != -1)
		{
			COLORID_Shaders.push_back(COLORID_FragmentShader);
		}


		// Create Core Program
		if (m_isGlobal)
		{
			m_coreProgram = GlobalAssets.createShaderProgram(name + "_program", CORE_Shaders);
			m_colorIDProgram = GlobalAssets.createShaderProgram(name + "_colorID_program", COLORID_Shaders);
		}
		else
		{
			m_coreProgram = SceneAssets.createShaderProgram(name + "_program", CORE_Shaders);
			m_colorIDProgram = SceneAssets.createShaderProgram(name + "_colorID_program", COLORID_Shaders);
		}

		// Data
		m_targetLevels = targetLevels;
	}

	ShaderProgram* ShaderMaterial::getProgram(ShaderMaterialType type)
	{
		switch (type)
		{
		case ShaderMaterialType::CORE:
			return Assets.getShaderProgram(m_coreProgram);
		case ShaderMaterialType::COLORID:
			return Assets.getShaderProgram(m_colorIDProgram);
		default:
			VXL_ERROR("Invalid Enum type for getting Program from ShaderMaterial");
			return nullptr;
		}
	}
}