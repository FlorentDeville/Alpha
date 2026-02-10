/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/Lights/PointLightComponent.h"

#include "Rendering/RenderModule.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	PointLightComponent::PointLightComponent()
		: GameComponent()
		, m_position(0, 0, 0, 0)
		, m_ambient(1, 1, 1, 1)
		, m_diffuse(1, 1, 1, 1)
		, m_specular(1, 1, 1, 1)
		, m_constant(1)
		, m_linear(0.22f)
		, m_quadratic(0.2f)
	{ }

	PointLightComponent::~PointLightComponent()
	{ }

	void PointLightComponent::Render()
	{ }
}
