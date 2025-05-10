/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Widgets/Tools/Color.h"
#include "Widgets/Widget.h"

#include <map>

namespace Widgets
{
	class AbstractViewModel;
	class Label;
	class Layout;
	class ModelIndex;
	class SelectionRow;

	class TableView : public Widget
	{
	public:
		TableView();
		~TableView();

		void SetModel(AbstractViewModel* pModel);

		void SetMultiSelection(bool enable);
		
		EVENT_DECL(ItemDoubleClick, void(const ModelIndex& idx))

	private:
		AbstractViewModel* m_pModel;

		Color m_oddRowBackgroundColor;
		Color m_evenRowBackgroundColor;
		Color m_hoverBackgroundColor;
		Color m_selectedBorderColor;

		Layout* m_pLayout;

		bool m_multiSelectionEnabled;

		void CreateView();

		void OnMouseDown_ItemLayout(const MouseEvent& ev, int itemIndex);
		void OnMouseDoubleClick_ItemLayout(const MouseEvent& ev, int row);

		void SetSelectedRowStyle(int row);
		void SetDeselectedRowStyle(int row);

		void OnCommitInsertRows(int start, int count, const ModelIndex& parent);
		void OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected);
		void OnDataChanged_SelectionModel(const ModelIndex& index);

		Layout* CreateItem(int row, int columnCount, const ModelIndex& parent);
		Label* GetItem(int row, int column, const ModelIndex& parent);
	};
}
