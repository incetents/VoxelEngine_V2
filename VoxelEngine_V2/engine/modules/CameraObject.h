// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "Entity.h"

namespace Vxl
{
	class CameraObject : public Entity
	{
	private:
		CameraObject(const std::string& name);

	public:
		// Destructor
		~CameraObject();

		// Database Creation
		static CameraObject* Create(const std::string& name);

		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}