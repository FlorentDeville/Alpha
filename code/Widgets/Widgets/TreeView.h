/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Core/Collections/Array.h"

#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Tools/Color.h"
#include "Widgets/Widget.h"

namespace Widgets
{
	class AbstractViewModel;
	class Icon;
	class Label;
	class Layout;
	class ModelIndex;
	class SelectionRow;
	class Split;

	class TreeView : public Widget
	{
	public:
		TreeView();
		~TreeView();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor);

		void SetModel(AbstractViewModel* pModel);

		void SetMultiSelection(bool enable);

		void SetColumnWidth(int column, int width);
		
		EVENT_DECL(ItemDoubleClick, void(const ModelIndex& idx))

	private:
		AbstractViewModel* m_pModel;

		Color m_oddRowBackgroundColor;
		Color m_evenRowBackgroundColor;
		Color m_defaultRowBackgroundColor;
		Color m_hoverBackgroundColor;
		Color m_selectedBorderColor;
		Color m_headerBackgroundColor;

		Layout* m_pLayout;

		bool m_multiSelectionEnabled;

		Core::UInt2 m_cellDefaultSize;

		Core::Array<uint32_t> m_columnWidth;

		struct RowInfo
		{
			int m_depth;
			ModelIndex m_index;
			bool m_collapsed;
			Icon* m_pIcon;
		};

		std::map<Widget*, RowInfo> m_rowInfoMap;
		std::map<ModelIndex, Layout*> m_modelIndexToRowMap;

		void CreateHeader();
		void CreateView();
		void CreateView_Recursive(const ModelIndex& parent, int depth);

		// Event callbacks
		void OnMouseDown_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);
		void OnMouseDoubleClick_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);

		void SetSelectedRowStyle(Layout* pLayout);
		void SetDeselectedRowStyle(Layout* pLayout);

		void OnCommitInsertRows(int start, int count, const ModelIndex& parent);
		void OnRemoveRows(int start, int count, const ModelIndex& parent);
		void OnDataChanged(const ModelIndex& index);

		void OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected);

		void OnClick_Icon(Layout* pRowLayout);

		// Internal functions
		Layout* CreateItem(int row, int columnCount, const ModelIndex& parent, int depth, bool hasChildren);
		Label* GetCell(const ModelIndex& index);
		Layout* GetRowWidget(int row, int column, const ModelIndex& parent);

		int GetRowIndex(const Layout* pLayout) const;

		// Convert a row index into the widget index.
		int GetRowLayoutIndex(int row) const;

		void HeaderSplit_OnDrag(const Core::Int2& mousePosition, Label* pHeader, int columnIndex);

		void HideRowsRecursively(const ModelIndex& indexToHide);
		void ShowRowsRecursively(const ModelIndex& indexToShow);
	};
}
