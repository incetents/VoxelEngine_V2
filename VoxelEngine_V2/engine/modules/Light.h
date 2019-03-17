// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Entity.h"

namespace Vxl
{
	class Light : public Entity
	{
	private:
		Light(const std::string& name);

	public:
		// Destructor
		~Light();

		// Database Creation
		static Light* Create(const std::string& name);

		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}