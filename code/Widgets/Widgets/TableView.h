/********************************************************************/
/* � 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Core/Collections/Array.h"

#include "Widgets/Tools/Color.h"
#include "Widgets/Widget.h"

namespace Widgets
{
	class AbstractViewModel;
	class Label;
	class Layout;
	class ModelIndex;
	class SelectionRow;
	class Split;

	class TableView : public Widget
	{
	public:
		TableView();
		~TableView();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor);

		void SetModel(AbstractViewModel* pModel);

		void SetMultiSelection(bool enable);

		void SetColumnWidth(int column, int width);
		
		EVENT_DECL(ItemDoubleClick, void(const ModelIndex& idx))

	private:
		AbstractViewModel* m_pModel;

		Color m_oddRowBackgroundColor;
		Color m_evenRowBackgroundColor;
		Color m_hoverBackgroundColor;
		Color m_defaultRowBackgroundColor;
		Color m_selectedBorderColor;
		Color m_headerBackgroundColor;

		Layout* m_pLayout;

		bool m_multiSelectionEnabled;

		Core::UInt2 m_cellDefaultSize;

		Core::Array<uint32_t> m_columnWidth;

		void CreateHeader();
		void CreateView();

		void OnMouseDown_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);
		void OnMouseDoubleClick_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);

		void SetSelectedRowStyle(int row);
		void SetDeselectedRowStyle(int row);

		void Model_OnCommitInsertRows(int start, int count, const ModelIndex& parent);
		void Model_OnBeforeRemoveRows(int start, int count, const ModelIndex& parent);
		void Model_OnAfterRemoveRows(int start, int count, const ModelIndex& parent);

		void OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected);
		void OnDataChanged_SelectionModel(const ModelIndex& index);

		Layout* CreateItem(int row, int columnCount, const ModelIndex& parent);
		Label* GetItem(int row, int column, const ModelIndex& parent);
		Layout* GetRowWidget(int row, int column, const ModelIndex& parent);

		int GetRowIndex(const Layout* pLayout) const;

		// Convert a row index into the widget index.
		int GetRowLayoutIndex(int row) const;

		void HeaderSplit_OnDrag(const Core::Int2& mousePosition, Label* pHeader, int columnIndex);
	};
}
