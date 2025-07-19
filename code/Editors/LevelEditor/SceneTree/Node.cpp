/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTree/Node.h"

namespace Editors
{
	Node::Node()
		: m_pParent(nullptr)
		, m_children()
		, m_guid(Core::Guid::GenerateNewGuid())
	{}

	Node::~Node()
	{
		for (Node* pChild : m_children)
			delete pChild;
	}

	const Node* Node::GetConstParent() const
	{
		return m_pParent;
	}

	Node* Node::GetParent()
	{
		return m_pParent;
	}

	const std::vector<Node*>& Node::GetConstChildren() const
	{
		return m_children;
	}

	std::vector<Node*>& Node::GetChildren()
	{
		return m_children;
	}

	const Core::Guid& Node::GetConstGuid() const
	{
		return m_guid;
	}

	Core::Guid& Node::GetGuid()
	{
		return m_guid;
	}

	void Node::SetParent(Node* pParent)
	{
		m_pParent = pParent;
	}

	void Node::AddChild(Node* pChild)
	{
		pChild->SetParent(this);
		m_children.push_back(pChild);
	}

	void Node::RemoveChild(Node* pChild)
	{
		std::vector<Node*>::iterator it = std::find(m_children.begin(), m_children.end(), pChild);
		if (it == m_children.end())
			return;

		m_children.erase(it);
		pChild->SetParent(nullptr);
	}

	Entity* Node::ToEntity()
	{
		return nullptr;
	}

	const Entity* Node::ToConstEntity() const
	{
		return nullptr;
	}
}
