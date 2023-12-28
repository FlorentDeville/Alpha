/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SelectionMgr.h"

namespace Editors
{
	SelectionMgr::SelectionMgr()
		: m_selection()
		, m_onItemAdded()
		, m_onItemRemoved()
		, m_onClear()
	{}

	SelectionMgr::~SelectionMgr()
	{}

	bool SelectionMgr::Add(const Os::Guid& guid)
	{
		if (IsSelected(guid))
			return false;

		m_selection.push_back(guid);
		m_onItemAdded(guid);
		return true;
	}

	bool SelectionMgr::Remove(const Os::Guid& guid)
	{
		if (!IsSelected(guid))
			return false;

		m_selection.remove(guid);
		m_onItemRemoved(guid);
		return true;
	}

	void SelectionMgr::Clear()
	{
		if (m_selection.empty())
			return;

		m_selection.clear();
		m_onClear();
	}

	bool SelectionMgr::IsSelected(const Os::Guid& guid) const
	{
		std::list<Os::Guid>::const_iterator it = std::find(m_selection.cbegin(), m_selection.cend(), guid);
		if (it == m_selection.cend())
			return false;

		return true;
	}

	const std::list<Os::Guid>& SelectionMgr::GetSelectionList() const
	{
		return m_selection;
	}

	Core::CallbackId SelectionMgr::OnItemAdded(const OnItemAddedEvent::Callback& callback)
	{
		return m_onItemAdded.Connect(callback);
	}

	Core::CallbackId SelectionMgr::OnItemRemoved(const OnItemRemovedEvent::Callback& callback)
	{
		return m_onItemRemoved.Connect(callback);
	}

	Core::CallbackId SelectionMgr::OnClear(const OnClearEvent::Callback& callback)
	{
		return m_onClear.Connect(callback);
	}
}
