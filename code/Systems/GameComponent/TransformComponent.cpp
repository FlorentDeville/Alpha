/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/GameComponent/TransformComponent.h"

#include "Systems/Objects/GameObject.h"

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

	void TransformComponent::SetParent(Systems::GameObject* pParentGo)
	{
		m_pParentGo = pParentGo;

		if (m_pParentGo)
			m_parent = m_pParentGo->GetGuid();
	}

	const Core::Array<Core::Guid>& TransformComponent::GetChildrenGuid() const
	{
		return m_children;
	}

	void TransformComponent::AddChild(const Core::Guid& child)
	{
		Core::Array<Core::Guid>::Iterator it = std::find(m_children.cbegin(), m_children.cend(), child);

		if(it == m_children.cend())
			m_children.PushBack(child);
	}

	void TransformComponent::RemoveChild(const Core::Guid& child)
	{
		m_children.Erase(child);
	}
}
