/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "TransformComponent.h"

TransformComponent::TransformComponent(GameEntity* pParent)
	: GameComponent(pParent)
	, m_transform()
{}

TransformComponent::~TransformComponent()
{}

void TransformComponent::Update()
{
	GameComponent::Update();
}

void TransformComponent::Render()
{
	GameComponent::Render();
}

void TransformComponent::SetTransform(const DirectX::XMMATRIX& transform)
{
	m_transform = transform;
}

const DirectX::XMMATRIX& TransformComponent::GetTransform() const
{
	return m_transform;
}
