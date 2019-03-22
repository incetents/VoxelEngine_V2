// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "CameraObject.h"

#include "../modules/RenderManager.h"

namespace Vxl
{
	CameraObject::CameraObject(const std::string& name)
		: Entity(name, EntityType::CAMERA)
	{

	}

	CameraObject::~CameraObject()
	{
		RenderManager.RemoveEntity(this);
	}

	CameraObject* CameraObject::Create(const std::string& name)
	{
		// Create Unique Name
		auto UniqueName = FixNameDuplicate(name);

		// Create
		CameraObject* _entity = new CameraObject(UniqueName);
		// Store in entity database
		AddToDatabase(UniqueName, _entity);
		Message_Created(UniqueName, _entity);
		// Return
		return _entity;
	}

	// Behaviour
	void CameraObject::Update()
	{

	}

	void CameraObject::Draw()
	{

	}

	void CameraObject::TransformChanged()
	{

	}
}