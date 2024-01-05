/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <list>

namespace Widgets
{
	class ModelIndex;
	class SelectionRange;

	class SelectionModel
	{
	public:
		SelectionModel();
		~SelectionModel();

		bool IsRowSelected(const SelectionRange& range) const;

		void SelectRow(const SelectionRange& parent);
		void DeselectRow(const SelectionRange& parent);

	private:
		std::list<SelectionRange> m_selection;
	};
}
