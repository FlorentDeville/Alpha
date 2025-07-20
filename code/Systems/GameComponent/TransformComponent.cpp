/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/TransformComponent.h"

namespace Systems
{
	TransformComponent::TransformComponent()
		: GameComponent()
		, m_localTransform()
		, m_parent()
		, m_children()
	{ }

	TransformComponent::~TransformComponent()
	{ }

	const Core::Mat44f& TransformComponent::GetLocalTx() const
	{
		return m_localTransform;
	}

	void TransformComponent::SetLocalTx(const Core::Mat44f& localTx)
	{
		m_localTransform = localTx;
	}

	const Core::Guid& TransformComponent::GetParentGuid() const
	{
		return m_parent;
	}

	const Core::Array<Core::Guid>& TransformComponent::GetChildrenGuid() const
	{
		return m_children;
	}
}
