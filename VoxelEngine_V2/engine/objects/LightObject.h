// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"

namespace Vxl
{
	class LightObject : public Entity
	{
	private:
		LightObject(const std::string& name);

	public:
		// Destructor
		~LightObject();

		// Database Creation
		static LightObject* Create(const std::string& name);

		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}