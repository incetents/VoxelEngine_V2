// Copyright(c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "CameraObject.h"

namespace Vxl
{
	CameraObject::CameraObject(const std::string& name)
		: Entity(name, EntityType::CAMERA)
	{

	}

	CameraObject::~CameraObject()
	{

	}

	CameraObject* CameraObject::Create(const std::string& name)
	{
		// Create
		CameraObject* _entity = new CameraObject(name);
		// Store in entity database
		m_database.Set(_entity);
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