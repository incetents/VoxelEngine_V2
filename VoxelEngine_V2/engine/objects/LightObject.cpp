// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "LightObject.h"

#include "../modules/RenderManager.h"
#include "../opengl/Mesh.h"
#include "../opengl/Geometry.h"

namespace Vxl
{
	LightObject::LightObject(const std::string& name)
		: Entity(name, EntityType::LIGHT)
	{
		m_mesh = Geometry.GetQuad();

		RenderManager.AddEntity(this);
	}

	LightObject::~LightObject()
	{
		RenderManager.RemoveEntity(this);
	}

	LightObject* LightObject::Create(const std::string& name)
	{
		// Create Unique Name
		auto UniqueName = FixNameDuplicate(name);

		// Create
		LightObject* _entity = new LightObject(UniqueName);
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
		m_mesh->Draw();
	}

	void LightObject::TransformChanged()
	{

	}
}