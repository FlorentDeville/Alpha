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

	void Light::MakeDirectionalLight(const Core::Float3& direction, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular)
	{
		m_direction = direction;
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_type = LightType::Directional;
	}

	LightsCBuffer::LightsCBuffer()
		: m_numLight(0)
	{ }

	Light* LightsCBuffer::AddLight()
	{
		if (m_numLight >= MAX_LIGHT_COUNT)
			return nullptr;

		Light* pLight = &m_lightArray[m_numLight];
		++m_numLight;
		return pLight;
	}
}
