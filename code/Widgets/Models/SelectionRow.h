/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/ModelIndex.h"

namespace Widgets
{
	/*
	* Represent a single selected row.
	*/
	class SelectionRow
	{
	public:
		SelectionRow(const ModelIndex& parent, int row);
		~SelectionRow();

		ModelIndex GetParent() const;
		int GetRow() const;

		bool operator==(const SelectionRow& other) const;
		bool operator!=(const SelectionRow& other) const;

	private:
		ModelIndex m_parentIndex;
		int m_row;
	};
}
