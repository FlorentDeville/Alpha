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
		SelectionRow(const ModelIndex& start, const ModelIndex& end);
		~SelectionRow();

		ModelIndex GetStartIndex() const;
		ModelIndex GetEndIndex() const;

		bool operator==(const SelectionRow& other) const;
		bool operator!=(const SelectionRow& other) const;

	private:
		ModelIndex m_startIndex;
		ModelIndex m_endIndex;
	};
}
