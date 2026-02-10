/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"
#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, SpotLightComponent)
	class SpotLightComponent : public GameComponent
	{
	public:
		SpotLightComponent();
		~SpotLightComponent();

		void Render() override;

	private:
		Core::Vec4f m_position;
		Core::Vec4f m_direction;

		Core::Vec4f m_ambient;
		Core::Vec4f m_diffuse;
		Core::Vec4f m_specular;

		float m_constant;
		float m_linear;
		float m_quadratic;

		float m_cutOff; //cut off angle in radians. Nothing is lit outside of this angle.

		START_REFLECTION(Systems::SpotLightComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_position)
			ADD_FIELD(m_direction)
			ADD_FIELD(m_ambient)
			ADD_FIELD(m_diffuse)
			ADD_FIELD(m_specular)
			ADD_FIELD(m_constant)
			ADD_FIELD(m_linear)
			ADD_FIELD(m_quadratic)
			ADD_FIELD(m_cutOff)
		END_REFLECTION()
	};
}
