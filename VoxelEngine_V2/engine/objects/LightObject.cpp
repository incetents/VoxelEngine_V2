// Copyright(c) 2020 Emmanuel Lajeunesse
#include "Precompiled.h"
//#include "LightObject.h"
//
//#include "../rendering/RenderManager.h"
//#include "../rendering/Mesh.h"
//#include "../rendering/Primitives.h"
//#include "../rendering/Debug.h"
//
//namespace Vxl
//{
//	LightObject::LightObject(const std::string& name, LightType type)
//		: Entity(name, EntityType::LIGHT)
//	{
//		RenderManager.AddEntity(this);
//
//		SetType(type);
//	}
//
//	LightObject::~LightObject()
//	{
//		RenderManager.RemoveEntity(this);
//
//		delete m_data;
//	}
//
//	// Utility
//	void LightObject::SetType(LightType type)
//	{
//		delete m_data;
//
//		switch (type)
//		{
//		case LightType::POINT:
//			m_data = new Light_Point();
//			break;
//		case LightType::SPOTLIGHT:
//			m_data = new Light_Spot();
//			break;
//		case LightType::DIRLIGHT:
//			m_data = new Light_Directional();
//			break;
//		}
//	}
//
//	//	LightObject* LightObject::Create(const std::string& name, Light::Type type)
//	//	{
//	//		// Create Unique Name
//	//		auto UniqueName = FixNameDuplicate(name);
//	//	
//	//		// Create
//	//		LightObject* _entity = new LightObject(UniqueName, type);
//	//	
//	//		// Store in entity database
//	//		AddNamedAsset(UniqueName, _entity, AssetMessage::CREATED);
//	//		// Return
//	//		return _entity;
//	//	}
//
//	// Behaviour
//	void LightObject::Update()
//	{
//
//	}
//
//	void LightObject::DrawSelection()
//	{
//		switch (GetLightType())
//		{
//			case LightType::POINT:
//			{
//				Light_Point* _light = (Light_Point*)m_data;
//				Debug.DrawWireframeSphere(m_transform.getWorldPosition(), Vector3(_light->m_radius), Color4F(_light->m_color, 0.1f));
//				break;
//			}
//		}
//	}
//
//	void LightObject::Draw()
//	{
//
//	}
//
//	void LightObject::TransformChanged()
//	{
//
//	}
//}