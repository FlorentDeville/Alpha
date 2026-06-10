/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"

#include "Systems/Game/Subsystems/Collision/Shapes/ShapeSphere.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, CollisionSphereComponent)

namespace Systems
{
	class CollisionSphereComponent : public GameComponent
	{
	public:
		CollisionSphereComponent();
		~CollisionSphereComponent();

		void OnStartGame() override;
		void OnDestroyGame() override;

		void Update(float dt) override;

		ShapeSphere& GetSphere();

		const Core::Vec4f& GetCenter() const;
		float GetRadius() const;

	private:
		Core::Vec4f m_center;
		float m_radius;

		START_REFLECTION(Systems::CollisionSphereComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_center)
			ADD_FIELD(m_radius)
		END_REFLECTION()

		ShapeSphere m_sphere;
		uint32_t m_index;
	};
}
