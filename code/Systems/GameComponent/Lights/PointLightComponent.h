/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Color/Color.h"
#include "Core/Math/Vec4f.h"
#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, PointLightComponent)
	class PointLightComponent : public GameComponent
	{
	public:
		PointLightComponent();
		~PointLightComponent();

		void Render() override;

		const Core::Vec4f& GetPosition() const;

		const Core::Color& GetAmbient() const;
		const Core::Color& GetDiffuse() const;
		const Core::Color& GetSpecular() const;
		
		float GetConstant() const;
		float GetLinear() const;
		float GetQuadratic() const;

	private:
		Core::Vec4f m_position;

		Core::Color m_ambient;
		Core::Color m_diffuse;
		Core::Color m_specular;

		float m_constant;
		float m_linear;
		float m_quadratic;

		START_REFLECTION(Systems::PointLightComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_position)
			ADD_FIELD(m_ambient)
			ADD_FIELD(m_diffuse)
			ADD_FIELD(m_specular)
			ADD_FIELD(m_constant)
			ADD_FIELD(m_linear)
			ADD_FIELD(m_quadratic)
		END_REFLECTION()
	};
}
