// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../math/Color.h"
#include "../math/Transform.h"

namespace Vxl
{
	// Enum
	enum class SceneNodeType
	{
		ENTITY,
		CAMERA,
		LIGHT
	};

	class Entity;
	class Camera;
	class Light;

	class SceneNode
	{
		friend class Transform;
	private:
		void TransformChanged();
	public:
		virtual ~SceneNode() {}

		SceneNodeIndex		m_uniqueID;
		std::string			m_name;
		const SceneNodeType m_type;
		bool				m_isActive = true;
		bool				m_isSelected = false;
		bool				m_useTransform = true;
		Transform			m_transform;
		Color3F				m_labelColor = Color3F(1, 1, 1); // Inspector

		// check if all parents are active
		bool IsFamilyActive();
		// Check if Editor can see this
		inline bool IsSelected(void) const
		{
			return m_isSelected;
		}

		// Getters
		Entity* getEntity(void) const;
		Camera* getCamera(void) const;
		Light*	getLight(void) const;
		//
		SceneNode(SceneNodeType type, const std::string& name)
			: m_type(type), m_name(name)
		{
			m_transform.m_sceneNode = this;
		}
	};
}