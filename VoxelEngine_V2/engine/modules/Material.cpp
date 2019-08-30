// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../textures/Texture2D.h"

#include "../rendering/Shader.h"
#include "../rendering/Debug.h"
#include "../rendering/Mesh.h"

#include "../modules/Entity.h"
#include "../objects/GameObject.h"

#include "../math/Transform.h"

#include "../utilities/Logger.h"
#include "../utilities/Macros.h"

namespace Vxl
{
	// Static
	std::map<UINT, Material*> Material::m_masterOrder;
	bool Material::m_masterOrderDirty = false;

	// Database Creation
	Material* Material::Create(const std::string& _name, UINT _order)
	{
		Material* _material = new Material(_name, _order);

		AddNamedAsset(_name, _material, AssetMessage::CREATED);

		return _material;
	}

	void Material::UpdateProperties()
	{
		VXL_ASSERT(m_shaderProgram, "Material has no Program");

		m_property_useModel.AcquireUniform(*m_shaderProgram);
		m_property_model.AcquireUniform(*m_shaderProgram);
		m_property_useInstancing.AcquireUniform(*m_shaderProgram);
		m_property_useTexture.AcquireUniform(*m_shaderProgram);
		m_property_color.AcquireUniform(*m_shaderProgram);
		m_property_tint.AcquireUniform(*m_shaderProgram);
		m_property_viewport.AcquireUniform(*m_shaderProgram);
		m_property_alpha.AcquireUniform(*m_shaderProgram);
		m_property_output.AcquireUniform(*m_shaderProgram);
	}

	// Setters
	void Material::SetProgram(const ShaderProgram& _program)
	{
		m_shaderProgram = &_program;

		UpdateProperties();
	}

	// Bind
	void Material::BindProgram()
	{
		VXL_ASSERT(m_shaderProgram, "Material missing Shader Program");

		// Bind Shader
		m_shaderProgram->Bind();
		
	}
	void Material::BindStates()
	{
		// Bind Modes
		Graphics::SetCullMode(m_CullType);
		Graphics::SetBlendState(m_BlendState);
		if (m_BlendState)
		{
			// Normal Blend modes
			Graphics::SetBlendMode(m_BlendFunc.source, m_BlendFunc.destination);
			// Special Blend modes
			for (auto& blendAttach : m_BlendFuncAttachments)
				Graphics::SetBlendMode(blendAttach.first, blendAttach.second.source, blendAttach.second.destination);

			Graphics::SetBlendEquation(m_BlendEq);
		}
		Graphics::SetDepthPassRule(m_DepthFunc);
		Graphics::SetDepthRead(m_DepthRead);
		Graphics::SetDepthWrite(m_DepthWrite);
		Graphics::SetWireframeState(m_Wireframe);
	}
	void Material::BindTextures()
	{
		// Bind Textures (ignore if wireframe mode is ON)
		if (m_Wireframe == false && m_sharedTextures)
		{
			TextureBinder::BindTextures();
		}
	}

}