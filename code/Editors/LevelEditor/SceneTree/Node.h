/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

#include "Core/Guid/Guid.h"

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

		const Core::Guid& GetConstGuid() const;
		Core::Guid& GetGuid();

		void SetParent(Node* pParent);
		void AddChild(Node* pChild);
		void RemoveChild(Node* pChild);

		virtual Entity* ToEntity();
		virtual const Entity* ToConstEntity() const;

	protected:
		Core::Guid m_guid;

	private:
		Node* m_pParent;
		std::vector<Node*> m_children;
	};
}
