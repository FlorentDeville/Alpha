/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Color/Color.h"
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

		const Core::Vec4f& GetPosition() const;
		const Core::Vec4f& GetDirection() const;

		const Core::Color& GetAmbient() const;
		const Core::Color& GetDiffuse() const;
		const Core::Color& GetSpecular() const;

		float GetConstant() const;
		float GetLinear() const;
		float GetQuadratic() const;

		float GetCutOff() const;
		float GetOuterCutOff() const;

	private:
		Core::Vec4f m_position;
		Core::Vec4f m_direction;

		Core::Color m_ambient;
		Core::Color m_diffuse;
		Core::Color m_specular;

		float m_constant;
		float m_linear;
		float m_quadratic;

		float m_cutOff; //cut off angle in radians.
		float m_outerCutOff; // nothing outside this angle is lit.

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
			ADD_FIELD(m_outerCutOff)
		END_REFLECTION()
	};
}
