/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/ConstantBuffer/LightsCBuffer.h"

namespace Rendering
{
	LightCBuffer::LightCBuffer()
		: m_position()
		, m_direction()
		, m_type(LightType::Unknown)
	{ }

	void LightCBuffer::MakeDirectionalLight(const Core::Float3& direction, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular)
	{
		m_direction = direction;
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_type = LightType::Directional;
	}

	void LightCBuffer::MakePointLight(const Core::Float3& position, const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular,
		float constant, float linear, float quadratic)
	{
		m_position = position;
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_constant = constant;
		m_linear = linear;
		m_quadratic = quadratic;
		m_type = LightType::Point;
	}

	void LightCBuffer::MakeSpotLight(const Core::Float3& position, const Core::Float3& direction,
		const Core::Float3& ambient, const Core::Float3& diffuse, const Core::Float3& specular,
		float constant, float linear, float quadratic, float cutOff, float outerCutOff)
	{
		m_position = position;
		m_direction = direction;
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_constant = constant;
		m_linear = linear;
		m_quadratic = quadratic;
		m_cutOff = cutOff;
		m_outerCutOff = outerCutOff;
		m_type = LightType::Spot;
	}

	LightsArrayCBuffer::LightsArrayCBuffer()
		: m_numLight(0)
	{ }

	LightCBuffer* LightsArrayCBuffer::AddLight()
	{
		if (m_numLight >= MAX_LIGHT_COUNT)
			return nullptr;

		LightCBuffer* pLight = &m_lightArray[m_numLight];
		++m_numLight;
		return pLight;
	}
}
