/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <list>

#include "Core/Callbacks/CallbackMacro.h"

#include "Core/Guid/Guid.h"

namespace Editors
{
	class SelectionMgr
	{
	public:
		SelectionMgr();
		~SelectionMgr();

		bool Add(const Core::Guid& guid);
		bool Remove(const Core::Guid& guid);
		void Clear();

		bool IsSelected(const Core::Guid& guid) const;

		const std::list<Core::Guid>& GetSelectionList() const;

		//callbacks
		EVENT_DECL(ItemAdded, void(const Core::Guid& guid))
		EVENT_DECL(ItemRemoved, void(const Core::Guid& guid))
		EVENT_DECL(Clear, void())

	private:
		std::list<Core::Guid> m_selection;
	};
}
