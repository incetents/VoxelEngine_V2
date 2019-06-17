// Copyright (c) 2019 Emmanuel Lajeunesse
#pragma once

#include "../modules/Entity.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Color.h"

#include "../utilities/Macros.h"

namespace Vxl
{
	
	class Light
	{
	public:
		enum class Type
		{
			POINT,
			SPOTLIGHT,
			DIRLIGHT
		};
	protected:
		const Type m_type;

	public:
		Light(Type type) : m_type(type){}
		virtual ~Light() {}

		const Type GetType(void) const
		{
			return m_type;
		}
	};

	/* POINT LIGHT */
	class Light_Point : public Light
	{
	public:
		Light_Point(float radius = 5.0f, Color3F color = Color3F::WHITE)
			: Light(Light::Type::POINT), m_radius(radius), m_color(color)
	{}
		float m_radius;
		Color3F m_color;
	};

	/* SPOT LIGHT */
	class Light_Spot : public Light
	{
	public:
		Light_Spot()
			: Light(Light::Type::SPOTLIGHT)
		{}
		
	};

	/* DIRECTIONAL LIGHT */
	class Light_Directional : public Light
	{
	public:
		Light_Directional()
			: Light(Light::Type::DIRLIGHT)
		{}

	};

	class LightObject : public Entity
	{
	private:
		LightObject(const std::string& name, Light::Type type);

		// Data
		Light* m_data = nullptr;

		// Utility
		void SetType(Light::Type type);

	public:
		// Destructor
		~LightObject();

		// Database Creation
		static LightObject* Create(const std::string& name, Light::Type type);

		// Utility
		const Light::Type GetLightType(void) const
		{
			return m_data->GetType();
		}
		Light* GetLight(void) const
		{
			VXL_ASSERT(m_data != nullptr, "LightObject missing data");
			return m_data;
		}

		// Behaviour
		virtual void Update() override;
		virtual void Draw() override;
	private:
		virtual void TransformChanged() override;
	};
}