/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <list>

namespace Widgets
{
	class ModelIndex;
	class SelectionRow;

	/*
	* Model keeping track of the list of selected rows
	*/
	class SelectionModel
	{
	public:
		SelectionModel();
		~SelectionModel();

		bool IsRowSelected(const SelectionRow& row) const;

		void SelectRow(const SelectionRow& row);
		void DeselectRow(const SelectionRow& row);

	private:
		std::list<SelectionRow> m_selectedRows;
	};
}
