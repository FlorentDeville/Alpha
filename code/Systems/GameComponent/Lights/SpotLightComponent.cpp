/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/Lights/SpotLightComponent.h"

#include "Rendering/RenderModule.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	SpotLightComponent::SpotLightComponent()
		: GameComponent()
		, m_position(0, 0, 0, 1)
		, m_direction(0, -1, 0, 0)
		, m_ambient(1, 1, 1, 1)
		, m_diffuse(1, 1, 1, 1)
		, m_specular(1, 1, 1, 1)
		, m_constant(1)
		, m_linear(0.22f)
		, m_quadratic(0.2f)
		, m_cutOff(0.34f)
	{
	}

	SpotLightComponent::~SpotLightComponent()
	{
	}

	void SpotLightComponent::Render()
	{
	}
}
