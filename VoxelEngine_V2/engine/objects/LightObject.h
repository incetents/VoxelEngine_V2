//// Copyright (c) 2019 Emmanuel Lajeunesse
//#pragma once
//
//#include "../modules/Entity.h"
//#include "../math/Vector.h"
//#include "../math/Color.h"
//
//#include "../utilities/Macros.h"
//
//namespace Vxl
//{
//	enum class LightType
//	{
//		POINT,
//		SPOTLIGHT,
//		DIRLIGHT
//	};
//
//	class Light
//	{
//	public:
//		Light(LightType type) : m_type(type){}
//		virtual ~Light() {}
//
//		const LightType m_type;
//	};
//
//	/* POINT LIGHT */
//	class Light_Point : public Light
//	{
//	public:
//		Light_Point(float radius = 5.0f, Color3F color = Color3F::WHITE)
//			: Light(LightType::POINT), m_radius(radius), m_color(color)
//	{}
//		float m_radius;
//		Color3F m_color;
//	};
//
//	/* SPOT LIGHT */
//	class Light_Spot : public Light
//	{
//	public:
//		Light_Spot()
//			: Light(LightType::SPOTLIGHT)
//		{}
//		
//	};
//
//	/* DIRECTIONAL LIGHT */
//	class Light_Directional : public Light
//	{
//	public:
//		Light_Directional()
//			: Light(LightType::DIRLIGHT)
//		{}
//
//	};
//
//	class LightObject : public Entity //, public Asset<LightObject>
//	{
//		DISALLOW_COPY_AND_ASSIGN(LightObject);
//		friend class _Assets;
//	private:
//		LightObject(const std::string& name, LightType type);
//
//		// Data
//		Light* m_data = nullptr;
//
//		// Utility
//		void SetType(LightType type);
//
//	public:
//		// Destructor
//		~LightObject();
//
//		// Database Creation
//		//static LightObject* Create(const std::string& name, Light::Type type);
//
//		// Utility
//		const LightType GetLightType(void) const
//		{
//			return m_data->m_type;
//		}
//		Light* GetLight(void) const
//		{
//			VXL_ASSERT(m_data != nullptr, "LightObject missing data");
//			return m_data;
//		}
//
//		// Behaviour
//		virtual void Update(void) override;
//		virtual void DrawSelection(void) override;
//		virtual void Draw(void) override;
//	private:
//		virtual void TransformChanged(void) override;
//	};
//}