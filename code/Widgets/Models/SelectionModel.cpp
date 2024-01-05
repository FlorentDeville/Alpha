/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Models/SelectionModel.h"

#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/SelectionRow.h"

namespace Widgets
{
	SelectionModel::SelectionModel()
		: m_selectedRows()
	{}

	SelectionModel::~SelectionModel()
	{}

	bool SelectionModel::IsRowSelected(const SelectionRow& row) const
	{
		for (const SelectionRow& selectedRow : m_selectedRows)
		{
			if (selectedRow == row)
				return true;
		}

		return false;
	}

	const std::list<SelectionRow>& SelectionModel::GetSelectedRows() const
	{
		return m_selectedRows;
	}

	void SelectionModel::SetSelectionRow(const SelectionRow& row)
	{
		m_selectedRows.clear();
		m_selectedRows.push_back(row);
	}

	void SelectionModel::SelectRow(const SelectionRow& row)
	{
		if (IsRowSelected(row))
			return;

		m_selectedRows.push_back(row);
	}

	void SelectionModel::DeselectRow(const SelectionRow& row)
	{
		for(std::list<SelectionRow>::const_iterator it = m_selectedRows.cbegin(); it != m_selectedRows.cend(); ++it)
		{
			const SelectionRow& selectedRow = *it;

			if (selectedRow != row)
				continue;

			m_selectedRows.erase(it);
			return;
		}
	}
}
