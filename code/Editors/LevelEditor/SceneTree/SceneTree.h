/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <functional>
#include <map>

namespace Core
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

		const Node* GetConstNode(const Core::Guid& guid) const;
		Node* GetNode(const Core::Guid& guid);

		bool AddNode(Node* pNode, const Core::Guid& parent);
		bool DeleteNode(const Core::Guid& guid);

		void Traverse(const std::function<void(const Node* pNode)>& callback) const;

	private:
		Node* m_pRoot;
		std::map<Core::Guid, Node*> m_nodeCache;

		void DeleteNodeRecursive(Node* pNode);

		void TraverseRecursive(const Node* pNode, const std::function<void(const Node* pNode)>& callback) const;
	};
}
