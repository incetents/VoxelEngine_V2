// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "SceneNode.h"

#include "../modules/Entity.h"

namespace Vxl
{
	// Check if Node should be used
	bool SceneNode::IsFamilyActive()
	{
		// if no parent, check normal active state
		Transform* parent = m_transform.getParent();
		if (parent == nullptr)
			return m_isActive;

		// auto fail if off
		if (!m_isActive)
			return false;

		// iterate through all parents
		while (parent != nullptr)
		{
			SceneNode* owner = parent->m_sceneNode;
			VXL_ASSERT(owner, "Transform missing SceneNode parent");
			if (!owner->m_isActive)
				return false;
			// Acquire new parent
			else
				parent = owner->m_transform.getParent();
		}
		// If no parents failed, that means it's all good
		return true;
	}

	// Getters
	Entity* SceneNode::getEntity(void) const
	{
		VXL_ASSERT(m_type == SceneNodeType::ENTITY, "Incorrect Scene Node Type");
		return (Entity*)this;
	}
	Camera* SceneNode::getCamera(void) const
	{
		VXL_ASSERT(m_type == SceneNodeType::CAMERA, "Incorrect Scene Node Type");
		return (Camera*)this;
	}
	Light* SceneNode::getLight(void) const
	{
		VXL_ASSERT(m_type == SceneNodeType::LIGHT, "Incorrect Scene Node Type");
		return (Light*)this;
	}

	void SceneNode::TransformChanged()
	{
		switch (m_type)
		{
		case SceneNodeType::ENTITY:
		{
			Entity* _entity = (Entity*)this;
			_entity->UpdateBoundingBoxCheap();
			break;
		}
		}
	}
}