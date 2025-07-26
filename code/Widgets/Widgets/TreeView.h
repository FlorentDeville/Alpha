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
		EVENT_DECL(DropItem, void(const ModelIndex& index, const ModelIndex& newParent)) //called when a row is dragged and dropped on another row

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
		
		bool m_isDragStarted;
		Layout* m_pDraggedRowLayout; // widget of the row that triggered the drag and drop event.
		ModelIndex m_dragModelIndex; // model index of the row being dragged

		Core::UInt2 m_cellDefaultSize;

		Core::Array<uint32_t> m_columnWidth;

		struct RowInfo
		{
			int m_depth;
			bool m_collapsed;
			Icon* m_pIcon;
			Layout* m_pParent;
		};

		std::map<const Widget*, RowInfo> m_rowInfoMap;
		std::map<const Layout*, Core::Array<Layout*>> m_rowLayoutTree;

		void CreateHeader();
		void CreateView();
		void CreateView_Recursive(const ModelIndex& parent, const Layout* pParentLayout, int depth);

		// Event callbacks
		void TreeView_OnMouseExit(const MouseEvent& ev);

		void OnMouseDown_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);
		void OnMouseUp_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout);
		void OnMouseMove_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);
		void OnMouseDoubleClick_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout);

		void SetSelectedRowStyle(Layout* pLayout);
		void SetDeselectedRowStyle(Layout* pLayout);

		void OnCommitInsertRows(int start, int count, const ModelIndex& parent);
		void Model_OnBeforeRemoveRows(int start, int count, const ModelIndex& parent);
		void Model_OnAfterRemoveRows(int start, int count, const ModelIndex& parent);
		void OnDataChanged(const ModelIndex& index);

		void OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected);

		void OnClick_Icon(Layout* pRowLayout);
		
		void HeaderSplit_OnDrag(const Core::Int2& mousePosition, Label* pHeader, int columnIndex);

		// Internal functions
		Layout* CreateItem(const ModelIndex& index, int columnCount, int depth);
		Label* GetCell(const ModelIndex& index);
		Layout* ComputeRowLayoutFromModelIndex(const ModelIndex& index);
		ModelIndex ComputeModelIndexFromRowLayout(const Layout* pLayout);

		//Return the index of the layout widget in the parent layout containing all the rows.
		int GetRowIndexInLayout(const Layout* pLayout) const;

		RowInfo* GetRowInfo(const Widget* pRowLayout);

		void HideRowsRecursively(const ModelIndex& indexToHide);
		void ShowRowsRecursively(const ModelIndex& indexToShow);

		void DeleteRowRecursively(Layout* pParentRow, Layout* pRowToRemove);
	};
}
