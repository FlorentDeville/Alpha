/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <functional>
#include <map>

namespace Os
{
	class Guid;
}

namespace Editors
{
	class Node;

	class SceneTree
	{
	public:
		SceneTree();
		~SceneTree();

		const Node* GetConstRoot() const;
		Node* GetRoot();

		const Node* GetConstNode(const Os::Guid& guid) const;
		Node* GetNode(const Os::Guid& guid);

		bool AddNode(Node* pNode, const Os::Guid& parent);
		bool DeleteNode(const Os::Guid& guid);

		void Traverse(const std::function<void(const Node* pNode)>& callback) const;

	private:
		Node* m_pRoot;
		std::map<Os::Guid, Node*> m_nodeCache;

		void DeleteNodeRecursive(Node* pNode);

		void TraverseRecursive(const Node* pNode, const std::function<void(const Node* pNode)>& callback) const;
	};
}
