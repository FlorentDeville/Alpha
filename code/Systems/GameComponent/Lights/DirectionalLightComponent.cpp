/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"

#include "Core/Math/Vectors.h"

#include "Rendering/RenderModule.h"

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	DirectionalLightComponent::DirectionalLightComponent()
		: GameComponent()
		, m_direction(1, -1, 1, 0)
		, m_ambient(1, 1, 1)
		, m_diffuse(1 ,1 , 1)
		, m_specular(1, 1, 1)
	{ }

	DirectionalLightComponent::~DirectionalLightComponent()
	{ }

	const Core::Vec4f& DirectionalLightComponent::GetDirection() const
	{
		return m_direction;
	}

	const Core::Color& DirectionalLightComponent::GetAmbient() const
	{
		return m_ambient;
	}

	const Core::Color& DirectionalLightComponent::GetDiffuse() const
	{
		return m_diffuse;
	}

	const Core::Color& DirectionalLightComponent::GetSpecular() const
	{
		return m_specular;
	}
}
