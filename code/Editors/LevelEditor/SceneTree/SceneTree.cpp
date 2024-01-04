/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTree/SceneTree.h"

#include "Editors/LevelEditor/SceneTree/Node.h"

namespace Editors
{
	SceneTree::SceneTree()
		: m_pRoot(nullptr)
		, m_nodeCache()
	{}

	SceneTree::~SceneTree()
	{
		delete m_pRoot;
		m_nodeCache.clear();
	}

	const Node* SceneTree::GetConstRoot() const
	{
		return m_pRoot;
	}

	Node* SceneTree::GetRoot()
	{
		return m_pRoot;
	}

	const Node* SceneTree::GetConstNode(const Os::Guid& guid) const
	{
		std::map<Os::Guid, Node*>::const_iterator it = m_nodeCache.find(guid);
		if (it == m_nodeCache.cend())
			return nullptr;

		return it->second;
	}

	Node* SceneTree::GetNode(const Os::Guid& guid)
	{
		std::map<Os::Guid, Node*>::iterator it = m_nodeCache.find(guid);
		if (it == m_nodeCache.end())
			return nullptr;

		return it->second;
	}

	bool SceneTree::AddNode(Node* pNode, const Os::Guid& parent)
	{
		if (!parent.IsValid()) //invalid parent then it's the root
		{
			m_pRoot = pNode;
		}
		else
		{
			Node* pParentNode = GetNode(parent);
			if (!pParentNode)
				return false;

			pParentNode->AddChild(pNode);
		}

		m_nodeCache[pNode->GetConstGuid()] = pNode;

		return true;
	}

	bool SceneTree::DeleteNode(const Os::Guid& guid)
	{
		Node* pNode = GetNode(guid);
		if (!pNode)
			return false;

		Node* pParent = pNode->GetParent();
		if (!pParent) //no parent then it's the root
		{
			delete m_pRoot;
			m_pRoot = nullptr;
			m_nodeCache.clear();
			return true;
		}

		pParent->RemoveChild(pNode);
		DeleteNodeRecursive(pNode);
		return true;
	}

	void SceneTree::Traverse(const std::function<void(const Node* pNode)>& callback) const
	{
		if (!m_pRoot)
			return;

		TraverseRecursive(m_pRoot, callback);
	}

	void SceneTree::DeleteNodeRecursive(Node* pNode)
	{
		std::vector<Node*>& children = pNode->GetChildren();
		for (Node* pChild : children)
		{
			DeleteNodeRecursive(pChild);			
		}

		children.clear();

		m_nodeCache.erase(pNode->GetConstGuid());
		delete pNode;
	}

	void SceneTree::TraverseRecursive(const Node* pNode, const std::function<void(const Node* pNode)>& callback) const
	{
		callback(pNode);

		const std::vector<Node*>& children = pNode->GetConstChildren();
		for (const Node* pChild : children)
		{
			if(pChild)
				TraverseRecursive(pChild, callback);
		}
	}
}