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
	{ }

	SpotLightComponent::~SpotLightComponent()
	{ }

	void SpotLightComponent::Render()
	{ }

	const Core::Vec4f& SpotLightComponent::GetPosition() const
	{
		return m_position;
	}

	const Core::Vec4f& SpotLightComponent::GetDirection() const
	{
		return m_direction;
	}

	const Core::Vec4f& SpotLightComponent::GetAmbient() const
	{
		return m_ambient;
	}

	const Core::Vec4f& SpotLightComponent::GetDiffuse() const
	{
		return m_diffuse;
	}

	const Core::Vec4f& SpotLightComponent::GetSpecular() const
	{
		return m_specular;
	}

	float SpotLightComponent::GetConstant() const
	{
		return m_constant;
	}

	float SpotLightComponent::GetLinear() const
	{
		return m_linear;
	}

	float SpotLightComponent::GetQuadratic() const
	{
		return m_quadratic;
	}

	float SpotLightComponent::GetCutOff() const
	{
		return m_cutOff;
	}

	float SpotLightComponent::GetOuterCutOff() const
	{
		return m_outerCutOff;
	}

}
