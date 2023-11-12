/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include "OsWin/Guid.h"

namespace Editors
{
	class Entity;

	class Node
	{
	public:
		Node();
		virtual ~Node();

		const Node* GetConstParent() const;
		Node* GetParent();

		const std::vector<Node*>& GetConstChildren() const;
		std::vector<Node*>& GetChildren();

		const Os::Guid& GetConstGuid() const;
		Os::Guid& GetGuid();

		void SetParent(Node* pParent);
		void AddChild(Node* pChild);
		void RemoveChild(Node* pChild);

		virtual Entity* ToEntity();
		virtual const Entity* ToConstEntity() const;

	private:
		Node* m_pParent;
		std::vector<Node*> m_children;

		Os::Guid m_guid;
	};
}
