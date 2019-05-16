// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LightObject.h"

#include "../modules/RenderManager.h"
#include "../opengl/Mesh.h"
#include "../opengl/Geometry.h"

namespace Vxl
{
	LightObject::LightObject(const std::string& name, Light::Type type)
		: Entity(name, EntityType::LIGHT)
	{
		RenderManager.AddEntity(this);

		SetType(type);
	}

	LightObject::~LightObject()
	{
		RenderManager.RemoveEntity(this);

		delete m_data;
	}

	// Utility
	void LightObject::SetType(Light::Type type)
	{
		delete m_data;

		switch (type)
		{
		case Light::Type::POINT:
			m_data = new Light_Point();
			break;
		case Light::Type::SPOTLIGHT:
			m_data = new Light_Spot();
			break;
		case Light::Type::DIRLIGHT:
			m_data = new Light_Directional();
			break;
		}
	}

	LightObject* LightObject::Create(const std::string& name, Light::Type type)
	{
		// Create Unique Name
		auto UniqueName = FixNameDuplicate(name);

		// Create
		LightObject* _entity = new LightObject(UniqueName, type);

		// Store in entity database
		AddToDatabase(UniqueName, _entity);
		Message_Created(UniqueName, _entity);
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