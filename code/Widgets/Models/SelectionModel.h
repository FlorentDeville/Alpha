/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include <list>
#include <vector>

namespace Widgets
{
	class AbstractViewModel;
	class ModelIndex;
	class SelectionRow;

	/*
	* Model keeping track of the list of selected rows
	*/
	class SelectionModel
	{
		friend AbstractViewModel;

	public:
		SelectionModel();
		~SelectionModel();

		bool IsRowSelected(const SelectionRow& row) const;

		const std::list<SelectionRow>& GetSelectedRows() const;

		void SetSelectionRow(const SelectionRow& row);
		void SelectRow(const SelectionRow& row);
		void DeselectRow(const SelectionRow& row);

		EVENT_DECL(SelectionChanged, void(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected));

	private:
		std::list<SelectionRow> m_selectedRows;

		void CommitInsertRows(int start, int count, const ModelIndex& parent);
	};
}
