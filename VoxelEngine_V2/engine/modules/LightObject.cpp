// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LightObject.h"

namespace Vxl
{
	LightObject::LightObject(const std::string& name)
		: Entity(name, EntityType::LIGHT)
	{

	}

	LightObject::~LightObject()
	{
	
	}

	LightObject* LightObject::Create(const std::string& name)
	{
		// Create
		LightObject* _entity = new LightObject(name);
		// Store in entity database
		m_database.Set(_entity);
		// Return
		return _entity;
	}

	// Behaviour
	void LightObject::Update()
	{

	}

	void LightObject::Draw()
	{

	}

	void LightObject::TransformChanged()
	{

	}
}