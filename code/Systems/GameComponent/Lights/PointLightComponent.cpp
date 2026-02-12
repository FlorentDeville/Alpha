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
		, m_ambient(1, 1, 1)
		, m_diffuse(1, 1, 1)
		, m_specular(1, 1, 1)
		, m_constant(1)
		, m_linear(0.22f)
		, m_quadratic(0.2f)
	{ }

	PointLightComponent::~PointLightComponent()
	{ }

	void PointLightComponent::Render()
	{ }

	const Core::Vec4f& PointLightComponent::GetPosition() const
	{
		return m_position;
	}

	const Core::Color& PointLightComponent::GetAmbient() const
	{
		return m_ambient;
	}

	const Core::Color& PointLightComponent::GetDiffuse() const
	{
		return m_diffuse;
	}

	const Core::Color& PointLightComponent::GetSpecular() const
	{
		return m_specular;
	}

	float PointLightComponent::GetConstant() const
	{
		return m_constant;
	}

	float PointLightComponent::GetLinear() const
	{
		return m_linear;
	}

	float PointLightComponent::GetQuadratic() const
	{
		return m_quadratic;
	}
}
