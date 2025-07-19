/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <list>

#include "Core/Callbacks/CallbackList.h"
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
		using OnItemAddedEvent = Core::CallbackList<void(const Core::Guid& nodeGuid)>;
		using OnItemRemovedEvent = Core::CallbackList<void(const Core::Guid& nodeGuid)>;
		using OnClearEvent = Core::CallbackList<void()>;

		Core::CallbackId OnItemAdded(const OnItemAddedEvent::Callback& callback);
		Core::CallbackId OnItemRemoved(const OnItemRemovedEvent::Callback& callback);
		Core::CallbackId OnClear(const OnClearEvent::Callback& callback);

	private:
		std::list<Core::Guid> m_selection;

		OnItemAddedEvent m_onItemAdded;
		OnItemRemovedEvent m_onItemRemoved;
		OnClearEvent m_onClear;
	};
}
