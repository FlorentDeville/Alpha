/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"
#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, DirectionalLightComponent)
	class DirectionalLightComponent : public GameComponent
	{
	public:
		DirectionalLightComponent();
		~DirectionalLightComponent();

		void Render() override;

		const Core::Vec4f& GetDirection() const;
		const Core::Vec4f& GetAmbient() const;
		const Core::Vec4f& GetDiffuse() const;
		const Core::Vec4f& GetSpecular() const;

	private:
		Core::Vec4f m_direction;

		Core::Vec4f m_ambient;
		Core::Vec4f m_diffuse;
		Core::Vec4f m_specular;

		START_REFLECTION(Systems::DirectionalLightComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_direction)
			ADD_FIELD(m_ambient)
			ADD_FIELD(m_diffuse)
			ADD_FIELD(m_specular)
		END_REFLECTION()
	};
}
