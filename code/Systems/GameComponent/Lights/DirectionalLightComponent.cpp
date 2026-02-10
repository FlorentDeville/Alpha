/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"

#include "Rendering/RenderModule.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	DirectionalLightComponent::DirectionalLightComponent()
		: GameComponent()
		, m_direction(1, -1, 1, 0)
		, m_ambient(1, 1, 1, 1)
		, m_diffuse(1 ,1 , 1, 1)
		, m_specular(1, 1, 1, 1)
	{ }

	DirectionalLightComponent::~DirectionalLightComponent()
	{ }

	void DirectionalLightComponent::Render()
	{
		const GameObject* pOwner = GetOwner();
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

		DirectX::XMFLOAT4 colorWhite(m_ambient.GetX(), m_ambient.GetY(), m_ambient.GetZ(), m_ambient.GetW());

		const Core::Mat44f& worldTx = pOwner->GetTransform().GetWorldTx();

		//render the starting point
		{
			float uniformScaling = 0.2f;
			Core::Mat44f scaling = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(uniformScaling, uniformScaling, uniformScaling, 0));
			Core::Mat44f world = scaling * worldTx;
			renderModule.RenderPrimitiveCube(world.m_matrix, colorWhite);
		}

		Core::Mat44f lookAt;
		{
			Core::Vec4f forward = m_direction;
			forward.Normalize();

			Core::Vec4f up(0, 1, 0, 0);
			Core::Vec4f right = up.Cross(forward);
			float rightDot = right.Dot(right);
			if (rightDot == 0)
			{
				up = Core::Vec4f(0, 0, 1, 0);
				right = up.Cross(forward);
			}

			up = forward.Cross(right);

			lookAt = Core::Mat44f(right, up, forward, worldTx.GetT());

			Core::Mat44f rot = Core::Mat44f::CreateRotationMatrix(Core::Vec4f(1, 0, 0, 0), 1.57f);
			lookAt = rot * lookAt;
		}

		//render the arrow
		float arrowLength = 2;
		{
			float halfArrowLength = arrowLength * 0.5f;
			Core::Mat44f translationOffset = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, halfArrowLength, 0, 1));
			Core::Mat44f scaling = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(0.1f, arrowLength, 0.1f, 0));

			Core::Mat44f arrowWorld = scaling * translationOffset * lookAt;

			renderModule.RenderPrimitiveCylinder(arrowWorld.m_matrix, colorWhite);
		}

		//render the arrow tip
		{
			Core::Mat44f translationOffset = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, arrowLength, 0, 1));
			Core::Mat44f scaling = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(0.5f, 1, 0.5f, 0));

			Core::Mat44f arrowTipWorld = scaling * translationOffset * lookAt;
			renderModule.RenderPrimitiveCone(arrowTipWorld.m_matrix, colorWhite);
		}
	}
}
