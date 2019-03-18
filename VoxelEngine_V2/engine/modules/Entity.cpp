// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"
#include "Entity.h"

#include "../opengl/glUtil.h"
#include "../modules/RenderManager.h"
#include "../utilities/Time.h"
#include "../utilities/logger.h"

#include <limits>

namespace Vxl
{
	DatabaseSet<Entity> Entity::m_database;

	Entity::Entity(const std::string& name, EntityType type)
		: m_type(type)
	{
		SetName(name);
		AddComponent(&m_transform, this);
	}

	Entity::~Entity()
	{
		RenderManager.RemoveEntity(this);
	}

	void Entity::Delete(Entity* _entity)
	{
		if (!_entity)
			return;

		RenderManager.RemoveEntity(_entity);
		m_database.Delete(_entity);
	}

	void Entity::SetName(const std::string _name)
	{
		if (_name.size() > MAX_ENTITY_NAME_LENGTH)
		{
			Logger.error("[Entity Error]");
			Logger.error("New name is too large,\n[Orig Name]: " + m_name + "\n[New Name]: " + _name);
			m_name = _name.substr(0, MAX_ENTITY_NAME_LENGTH);
		}
		else
			m_name = _name;
	}
}

