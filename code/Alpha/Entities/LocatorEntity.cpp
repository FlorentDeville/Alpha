/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Entities/LocatorEntity.h"

#include "Components/TransformComponent.h"

LocatorEntity::LocatorEntity()
{
	m_pTransform = new TransformComponent(this);
	AddGameComponent(m_pTransform);
}

LocatorEntity::~LocatorEntity()
{}

const DirectX::XMMATRIX& LocatorEntity::GetTransform() const
{
	return m_pTransform->GetTransform();
}

void LocatorEntity::SetTransform(const DirectX::XMMATRIX& transform)
{
	m_pTransform->SetTransform(transform);
}
