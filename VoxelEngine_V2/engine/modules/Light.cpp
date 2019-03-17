// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Light.h"

namespace Vxl
{
	Light::Light(const std::string& name)
		: Entity(name, EntityType::LIGHT)
	{

	}

	Light::~Light()
	{
	
	}

	Light* Light::Create(const std::string& name)
	{
		// Create
		Light* _entity = new Light(name);
		// Store in entity database
		m_database.Set(_entity);
		// Return
		return _entity;
	}

	// Behaviour
	void Light::Update()
	{

	}

	void Light::Draw()
	{

	}

	void Light::TransformChanged()
	{

	}
}