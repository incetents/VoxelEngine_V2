// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Material.h"

#include "../textures/Texture.h"

#include "../opengl/Shader.h"
#include "../opengl/Debug.h"
#include "../opengl/Mesh.h"

#include "../modules/Entity.h"
#include "../objects/GameObject.h"

#include "../math/Transform.h"

#include "../utilities/logger.h"
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

		AddToDatabase(_name, _material);
		Message_Created(_name, _material);

		return _material;
	}

	void Material::UpdateProperties()
	{
		VXL_ASSERT(m_shaderProgram, "Material has no Program");

		m_property_useModel.AcquireUniform(*m_shaderProgram);
		m_property_model.AcquireUniform(*m_shaderProgram);
		m_property_useInstancing.AcquireUniform(*m_shaderProgram);
		m_property_useColor.AcquireUniform(*m_shaderProgram);
		m_property_color.AcquireUniform(*m_shaderProgram);
		m_property_tint.AcquireUniform(*m_shaderProgram);
		m_property_viewport.AcquireUniform(*m_shaderProgram);
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
		glUtil::cullMode(m_CullType);
		glUtil::blendState(m_BlendState);
		if (m_BlendState)
		{
			glUtil::blendMode(m_BlendSource, m_BlendDest);
			glUtil::blendEquation(m_BlendEq);
		}
		glUtil::depthTest(m_DepthFunc);
		glUtil::depthState(m_DepthRead);
		glUtil::depthMask(m_DepthWrite);
		glUtil::wireframe(m_Wireframe);
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