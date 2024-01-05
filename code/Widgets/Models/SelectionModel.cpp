/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionModel.h"

#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/SelectionRange.h"

namespace Widgets
{
	SelectionModel::SelectionModel()
		: m_selection()
	{}

	SelectionModel::~SelectionModel()
	{}

	bool SelectionModel::IsRowSelected(const SelectionRange& range) const
	{
		int columnCount = range.GetConstModel()->GetColumnCount(range.GetParent());
		int lastColumnIndex = columnCount - 1;

		for (std::list<SelectionRange>::const_iterator it = m_selection.cbegin(); it != m_selection.cend(); ++it)
		{
			const SelectionRange& selectedRange = *it;

			if (selectedRange.GetParent() != range.GetParent())
				return false;

			if (selectedRange.GetLeft() == 0 && selectedRange.GetRight() == lastColumnIndex && selectedRange.GetTop() >= range.GetTop() && selectedRange.GetBottom() <= range.GetTop())
			{
				return true;
			}
		}

		return false;
	}

	void SelectionModel::SelectRow(const SelectionRange& range)
	{
		if (IsRowSelected(range))
			return;

		m_selection.push_back(range);
	}

	void SelectionModel::DeselectRow(const SelectionRange& range)
	{
		int columnCount = range.GetConstModel()->GetColumnCount(range.GetParent());
		int lastColumnIndex = columnCount - 1;

		for(std::list<SelectionRange>::const_iterator it = m_selection.cbegin(); it != m_selection.cend(); ++it)
		{
			const SelectionRange& selectedRange = *it;

			if (selectedRange.GetParent() != range.GetParent())
				continue;

			if (selectedRange.GetLeft() == 0 && selectedRange.GetRight() == lastColumnIndex && selectedRange.GetTop() >= range.GetTop() && selectedRange.GetBottom() <= range.GetTop())
			{
				m_selection.erase(it);
				return;
			}
		}
	}

}
