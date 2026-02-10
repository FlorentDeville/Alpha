/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/LightsCBuffer.h"

namespace Rendering
{
	Light::Light()
		: m_position()
		, m_direction()
		, m_type(LightType::Unknown)
	{ }

	Light Light::MakeDirectionalLight(const Core::Float3& direction, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular)
	{
		Light newLight;
		newLight.m_direction = direction;
		newLight.m_ambient = ambient;
		newLight.m_diffuse = diffuse;
		newLight.m_specular = specular;
		newLight.m_type = LightType::Directional;
		return newLight;
	}

	LightsCBuffer::LightsCBuffer()
		: m_numLight(0)
	{ }

	LightsCBuffer::LightsCBuffer(const Light& light)
	{
		m_numLight = 1;
		m_lightArray[0] = light;
	}
}
