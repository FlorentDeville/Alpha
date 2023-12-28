/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <list>

#include "Core/Callbacks/CallbackList.h"
#include "OsWin/Guid.h"

namespace Editors
{
	class SelectionMgr
	{
	public:
		SelectionMgr();
		~SelectionMgr();

		bool Add(const Os::Guid& guid);
		bool Remove(const Os::Guid& guid);
		void Clear();

		bool IsSelected(const Os::Guid& guid) const;

		const std::list<Os::Guid>& GetSelectionList() const;

		//callbacks
		using OnItemAddedEvent = Core::CallbackList<void(const Os::Guid& nodeGuid)>;
		using OnItemRemovedEvent = Core::CallbackList<void(const Os::Guid& nodeGuid)>;
		using OnClearEvent = Core::CallbackList<void()>;

		Core::CallbackId OnItemAdded(const OnItemAddedEvent::Callback& callback);
		Core::CallbackId OnItemRemoved(const OnItemRemovedEvent::Callback& callback);
		Core::CallbackId OnClear(const OnClearEvent::Callback& callback);

	private:
		std::list<Os::Guid> m_selection;

		OnItemAddedEvent m_onItemAdded;
		OnItemRemovedEvent m_onItemRemoved;
		OnClearEvent m_onClear;
	};
}
