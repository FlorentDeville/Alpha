/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/LightsCBuffer.h"

namespace Rendering
{
	Light::Light()
		: m_position()
		, m_direction()
		, m_coneAngle(0)
		, m_type(LightType::Directional)
	{ }

	Light Light::MakeDirectionalLight(const Core::Float3& direction)
	{
		Light newLight;
		newLight.m_direction = direction;
		newLight.m_type = LightType::Directional;
		return newLight;
	}

	LightsCBuffer::LightsCBuffer(const Light& light)
	{
		m_numLight = 1;
		m_lightArray[0] = light;
	}
}
