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
		, m_pParentGo(nullptr)
	{ }

	TransformComponent::~TransformComponent()
	{ }

	const Core::Mat44f& TransformComponent::GetLocalTx() const
	{
		return m_localTransform;
	}

	const Core::Mat44f& TransformComponent::GetWorldTx() const
	{
		return m_wsTransform;
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

	void TransformComponent::AddChild(Systems::GameObject* pChild)
	{
		Core::Array<Core::Guid>::Iterator it = std::find(m_children.cbegin(), m_children.cend(), pChild->GetGuid());

		if (it != m_children.cend())
			return;

		m_children.PushBack(pChild->GetGuid());
		m_childrenGo.PushBack(pChild);
	}

	void TransformComponent::RemoveChild(const Core::Guid& child)
	{
		m_children.Erase(child);

		Systems::GameObject* pChildGo = nullptr;
		for (Systems::GameObject* pGo : m_childrenGo)
		{
			if (pGo->GetGuid() == child)
			{
				pChildGo = pGo;
				break;
			}
		}

		if(pChildGo)
			m_childrenGo.Erase(pChildGo);
	}

	void TransformComponent::AddChildCachedPointer(Systems::GameObject* pGo)
	{
		m_childrenGo.PushBack(pGo);
	}

	void TransformComponent::Update()
	{
		if (!m_parent.IsValid())
		{
			m_wsTransform = m_localTransform;
		}
		else if (m_pParentGo)
		{
			const Core::Mat44f& parentWorld = m_pParentGo->GetTransform().GetWorldTx();
			m_wsTransform = m_localTransform * parentWorld;
		}

		for (Systems::GameObject* pChildGo : m_childrenGo)
			pChildGo->UpdateTransform();
	}
}
