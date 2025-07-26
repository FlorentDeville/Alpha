/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TreeView.h"

#include "OsWin/Cursor/Cursor.h"
#include "OsWin/Input.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"

#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/Widgets/TableViewStyle.h"

#include "Widgets/Container.h"
#include "Widgets/Icon.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Split.h"
#include "Widgets/WidgetMgr.h"

#include <stack>

extern OsWin::CursorId g_pIconName;

namespace Widgets
{
	TreeView::TreeView()
		: Widget()
		, m_pModel(nullptr)
		, m_oddRowBackgroundColor(TableViewStyle::s_oddRowBackgroundColor)
		, m_evenRowBackgroundColor(TableViewStyle::s_evenRowBackgroundColor)
		, m_hoverBackgroundColor(TableViewStyle::s_hoverBackgroundColor)
		, m_selectedBorderColor(TableViewStyle::s_selectedBorderColor)
		, m_headerBackgroundColor(TableViewStyle::s_headerBackgroundColor)
		, m_defaultRowBackgroundColor(0.f, 0.f, 0.f, 0.f)
		, m_multiSelectionEnabled(false)
		, m_cellDefaultSize(75, 20)
		, m_columnWidth()
		, m_rowLayoutTree()
		, m_isDragStarted(false)
		, m_dragModelIndex()
		, m_pDraggedRowLayout(nullptr)
	{
		SetSizeStyle(Widgets::Widget::STRETCH);

		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(SIZE_STYLE::STRETCH);
		m_pLayout->SetDirection(Layout::Vertical);
		m_pLayout->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
		m_pLayout->GetHoverStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
		AddWidget(m_pLayout);

		OnMouseExit([this](const Widgets::MouseEvent& ev) { TreeView_OnMouseExit(ev); });
	}

	TreeView::~TreeView()
	{
		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}
	}

	void TreeView::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		int valueShowBorder = m_showBorder ? 1 : 0;
		float rect[2] = { (float)m_size.x, (float)m_size.y };

		{
			WidgetMgr& widgetMgr = WidgetMgr::Get();
			Rendering::RenderModule& render = Rendering::RenderModule::Get();
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

			render.SetScissorRectangle(scissor);

			const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.GetWidgetMaterialId());
			render.BindMaterial(*pMaterial, wvp);

			render.SetConstantBuffer(1, sizeof(m_backgroundColor), &m_backgroundColor, 0);
			render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
			render.SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);
			render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
			render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

			struct AlternateColor
			{
				Color m_color1;
				Color m_color2;
				uint32_t m_size;
			};

			AlternateColor altColor;
			altColor.m_color1 = m_evenRowBackgroundColor;
			altColor.m_color2 = m_oddRowBackgroundColor;
			altColor.m_size = 20;
			render.SetConstantBuffer(6, sizeof(AlternateColor), &altColor, 0);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.GetQuadMeshId());
			render.RenderMesh(*pMesh);

			altColor.m_size = 0;
			render.SetConstantBuffer(6, sizeof(AlternateColor), &altColor, 0);
		}

		for (Widget* pWidget : m_children)
		{
			if (!pWidget->IsEnabled())
				continue;

			pWidget->Draw(windowSize, scissor);
		}
	}

	void TreeView::SetModel(AbstractViewModel* pModel)
	{
		if (m_pModel == pModel)
			return;

		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}

		m_pModel = pModel;
		m_pModel->OnCommitInsertRows([this](int start, int count, const ModelIndex& parent) { OnCommitInsertRows(start, count, parent); });
		m_pModel->OnBeforeRemoveRows([this](int start, int count, const ModelIndex& parent) { Model_OnBeforeRemoveRows(start, count, parent); });
		m_pModel->OnAfterRemoveRows([this](int start, int count, const ModelIndex& parent) { Model_OnAfterRemoveRows(start, count, parent); });
		m_pModel->OnDataChanged([this](const ModelIndex& index) { OnDataChanged(index); });

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected) { OnSelectionChanged_SelectionModel(selected, deselected); });

		m_columnWidth.Resize(static_cast<uint32_t>(m_pModel->GetColumnCount(Widgets::ModelIndex())), m_cellDefaultSize.x);

		CreateHeader();
		CreateView();
	}

	void TreeView::SetMultiSelection(bool enable)
	{
		m_multiSelectionEnabled = enable;
	}

	void TreeView::SetColumnWidth(int column, int width)
	{
		if (column < 0)
			return;

		if (!m_columnWidth.IsValidIndex(column))
			m_columnWidth.Resize(column + 1, m_cellDefaultSize.x);

		m_columnWidth[column] = width;

		std::vector<Widgets::Widget*> rows = m_pLayout->GetChildren();
		
		if (rows.size() < 1)
			return;

		//first resize the header
		{
			Widgets::Widget* pHeaderRow = rows[0];
			Widgets::Widget* pCell = pHeaderRow->GetChildren()[column * 2];
			pCell->SetWidth(width);
		}

		//now resize the rows
		for(size_t ii = 1; ii < rows.size(); ++ii)
		{
			Widgets::Widget* pRow = rows[ii];
			Widgets::Widget* pCell = pRow->GetChildren()[column];
			Core::UInt2 size = pCell->GetSize();
			size.x = width;
			pCell->SetSize(size);
		}
	}

	void TreeView::CreateHeader()
	{
		Layout* pHeaderLayout = new Layout();
		pHeaderLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
		pHeaderLayout->SetDirection(Layout::Horizontal);
		pHeaderLayout->GetDefaultStyle().SetBackgroundColor(m_headerBackgroundColor);
		pHeaderLayout->GetHoverStyle().SetBackgroundColor(m_headerBackgroundColor);

		int columnCount = m_pModel->GetColumnCount(Widgets::ModelIndex());
		for (int jj = 0; jj < columnCount; ++jj)
		{
			std::string data = "  " + m_pModel->GetHeaderData(jj);

			Label* pLabel = new Label(data);
			if (jj != columnCount - 1)
			{
				pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);

				if(m_columnWidth.IsValidIndex(jj))
					pLabel->SetSize(Core::UInt2(m_columnWidth[jj], m_cellDefaultSize.y));
				else
					pLabel->SetSize(m_cellDefaultSize);

				pHeaderLayout->AddWidget(pLabel);

				Split* pSplit = new Split(true);
				pSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
				pSplit->SetWidth(4);
				pSplit->SetBackgroundColor(Color(61, 61, 61));
				pSplit->OnDrag([this, pLabel, jj](const Core::Int2& mousePosition) { HeaderSplit_OnDrag(mousePosition, pLabel, jj);	});

				pHeaderLayout->AddWidget(pSplit);
			}
			else
			{
				pLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);

				pHeaderLayout->AddWidget(pLabel);
			}
		}

		m_pLayout->AddWidget(pHeaderLayout);
	}

	void TreeView::CreateView()
	{
		ModelIndex root;

		m_rowLayoutTree[nullptr];
		CreateView_Recursive(root, nullptr, 0);
	}

	void TreeView::CreateView_Recursive(const ModelIndex& parent, const Layout* pParentLayout, int depth)
	{
		int rowCount = m_pModel->GetRowCount(parent);
		int columnCount = m_pModel->GetColumnCount(parent);

		Core::Array<Layout*>& layoutArray = m_rowLayoutTree[pParentLayout];

		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, parent);

			Layout* pRowLayout = CreateItem(childIndex, columnCount, depth);
			m_pLayout->AddWidget(pRowLayout);

			layoutArray.PushBack(pRowLayout);
			CreateView_Recursive(childIndex, pRowLayout, depth + 1);
		}
	}

	void TreeView::TreeView_OnMouseExit(const MouseEvent& ev)
	{
		if (!m_isDragStarted)
			return;
		
		m_isDragStarted = false;
		g_pIconName = OsWin::CursorId::Arrow;
	}

	void TreeView::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		ModelIndex start = ComputeModelIndexFromRowLayout(pRowLayout);
		if (!start.IsValid())
			return;

		ModelIndex end = m_pModel->GetIndex(start.GetRow(), m_pModel->GetColumnCount(ModelIndex()) - 1, start.GetParent());
		SelectionRow clickedRow(start, end);

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		bool rowSelected = pSelectionModel->IsRowSelected(clickedRow);

		bool addToSelection = false;
		bool removeFromSelection = false;
		bool setSelection = false;
		if (Os::IsKeyDown(Os::VKeyCodes::Control) && m_multiSelectionEnabled) //ctrl+click does a toggle and adds to the current selection
		{
			if (rowSelected)
				removeFromSelection = true;
			else
				addToSelection = true;
		}
		else //regular click
		{
			if (!rowSelected)
				setSelection = true;
		}

		if (addToSelection)
		{
			SetSelectedRowStyle(pRowLayout);
			pSelectionModel->SelectRow(clickedRow);
		}

		if (removeFromSelection)
		{
			SetDeselectedRowStyle(pRowLayout);
			pSelectionModel->DeselectRow(clickedRow);
		}

		if(setSelection)
		{
			const std::list<SelectionRow>& selection = pSelectionModel->GetSelectedRows();
			for (const SelectionRow& sel : selection)
			{
				if(Layout* pSelectedLayout = ComputeRowLayoutFromModelIndex(sel.GetStartIndex()))
					SetDeselectedRowStyle(pSelectedLayout);
			}
			SetSelectedRowStyle(pRowLayout);
			pSelectionModel->SetSelectionRow(clickedRow);
		}
	}

	void TreeView::OnMouseUp_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		if (!m_isDragStarted)
			return;

		m_isDragStarted = false;
		g_pIconName = OsWin::CursorId::Arrow;

		if (m_pDraggedRowLayout == pRowLayout) // I can't reparent to myself
			return;

		ModelIndex parent = ComputeModelIndexFromRowLayout(pRowLayout);
		if (!parent.IsValid())
			return;

		m_onDropItem(m_dragModelIndex, parent);
	}

	void TreeView::OnMouseMove_ItemLayout(const MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		if (ev.GetButton() != Widgets::MouseButton::LeftButton)
			return;

		if (!m_isDragStarted)
		{
			ModelIndex start = ComputeModelIndexFromRowLayout(pRowLayout);
			if (!start.IsValid())
				return;

			m_isDragStarted = true;
			m_dragModelIndex = start;
			m_pDraggedRowLayout = pRowLayout;

			g_pIconName = OsWin::CursorId::Move;
			OsWin::SetCursor(g_pIconName);
		}
	}

	void TreeView::OnMouseDoubleClick_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		//int row = GetRowIndex(pRowLayout);
		//if (row < 0)
		//	return;

		//ModelIndex idx = m_pModel->GetIndex(row, 0, ModelIndex());
		//m_onItemDoubleClick(idx);
	}

	void TreeView::SetSelectedRowStyle(Layout* pLayout)
	{
		pLayout->GetDefaultStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderColor(m_selectedBorderColor);

		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBorderColor(m_selectedBorderColor);
	}

	void TreeView::SetDeselectedRowStyle(Layout* pLayout)
	{

		pLayout->GetDefaultStyle().SetBackgroundColor(m_defaultRowBackgroundColor);
		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(false);
		pLayout->GetDefaultStyle().ShowBorder(false);
	}

	void TreeView::OnCommitInsertRows(int start, int count, const ModelIndex& parent)
	{
		ModelIndex root;
		int columnCount = m_pModel->GetColumnCount(root);
		
		//compute the depth. not great.
		int depth = 0;
		ModelIndex temp = parent;
		while (temp.IsValid())
		{
			temp = temp.GetParent();
			++depth;
		}

		for (int ii = start; ii < start + count; ++ii)
		{
			Widgets::ModelIndex newIndex = m_pModel->GetIndex(ii, 0, parent);

			Layout* pRowLayout = CreateItem(newIndex, columnCount, depth);

			// I need to calculate the correct position for this row in the layout.
			// It needs to go after all the children of the previous sibling. It's easier to do
			// the reverse and to add it before the next sibling of my parent
			Widgets::ModelIndex followingSibling;
			Widgets::ModelIndex tempParentIndex = parent;
			while (tempParentIndex.IsValid())
			{
				Widgets::ModelIndex grandParent = tempParentIndex.GetParent();
				followingSibling = m_pModel->GetIndex(tempParentIndex.GetRow() + 1, 0, grandParent);
				if (followingSibling.IsValid())
					break;

				tempParentIndex = grandParent;
			}
			
			if (!followingSibling.IsValid()) // no following sibling, add the row at the end
			{
				m_pLayout->AddWidget(pRowLayout);
			}
			else
			{
				//find the index of the following rows. Ugly as fuck! I need a cache or something to do this
				Layout* pFollowingLayout = ComputeRowLayoutFromModelIndex(followingSibling);
				if (pFollowingLayout)
				{
					int layoutIndex = GetRowIndexInLayout(pFollowingLayout);
					m_pLayout->InsertWidget(pRowLayout, layoutIndex);
				}
			}

			Layout* pParentLayout = ComputeRowLayoutFromModelIndex(parent);

			//update the parent collapsed icon
			if(pParentLayout)
			{
				RowInfo& info = m_rowInfoMap[pParentLayout];
				info.m_collapsed = false;
				info.m_pIcon->Show();
			}

			Core::Array<Layout*>& children = m_rowLayoutTree[pParentLayout];
			children.PushBack(pRowLayout);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TreeView::Model_OnBeforeRemoveRows(int start, int count, const ModelIndex& parent)
	{
		//First delete the layout widget
		Layout* pParentRow = ComputeRowLayoutFromModelIndex(parent);
		for (int ii = start; ii < start + count; ++ii)
		{
			ModelIndex indexToRemove = m_pModel->GetIndex(ii, 0, parent);
			Widget* pWidgetToDelete = ComputeRowLayoutFromModelIndex(indexToRemove);
			Layout* pLayoutToDelete = static_cast<Layout*>(pWidgetToDelete);

			DeleteRowRecursively(pParentRow, pLayoutToDelete);
		}		
	}

	void TreeView::Model_OnAfterRemoveRows(int start, int count, const ModelIndex& parent)
	{
		//update the parent collapse state and icon
		int rowCount = m_pModel->GetRowCount(parent);
		bool hasChildren = rowCount > 0;
		if (!hasChildren)
		{
			Layout* pParentRow = ComputeRowLayoutFromModelIndex(parent);
			if (RowInfo* pInfo = GetRowInfo(pParentRow))
				pInfo->m_pIcon->Hide();
		}

		WidgetMgr::Get().RequestResize();
	}

	void TreeView::OnDataChanged(const ModelIndex& index)
	{
		Label* pLabel = GetCell(index);
		if (!pLabel)
			return;

		std::string value = m_pModel->GetData(index);
		pLabel->SetText(value);
	}

	void TreeView::OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected)
	{
		for (const SelectionRow& deselectedRow : deselected)
		{
			if(Layout* pLayout = ComputeRowLayoutFromModelIndex(deselectedRow.GetStartIndex()))
				SetDeselectedRowStyle(pLayout);
		}

		for (const SelectionRow& selectedRow : selected)
		{
			if (Layout* pLayout = ComputeRowLayoutFromModelIndex(selectedRow.GetStartIndex()))
				SetSelectedRowStyle(pLayout);
		}
	}

	void TreeView::OnClick_Icon(Layout* pRowLayout)
	{
		RowInfo* pInfo = GetRowInfo(pRowLayout);
		if (!pInfo)
			return;

		ModelIndex index = ComputeModelIndexFromRowLayout(pRowLayout);

		if (pInfo->m_collapsed)
		{
			Rendering::TextureId expandedIcon = WidgetMgr::Get().GetIconTextureId(Widgets::IconId::kIconExpanded);
			pInfo->m_pIcon->SetTextureId(expandedIcon);

			int rowCount = m_pModel->GetRowCount(index);
			for (int ii = 0; ii < rowCount; ++ii)
			{
				ModelIndex childIndex = m_pModel->GetIndex(ii, 0, index);
				ShowRowsRecursively(childIndex);
			}
		}
		else
		{
			Rendering::TextureId collapsedIcon = WidgetMgr::Get().GetIconTextureId(Widgets::IconId::kIconCollapsed);
			pInfo->m_pIcon->SetTextureId(collapsedIcon);

			int rowCount = m_pModel->GetRowCount(index);
			for (int ii = 0; ii < rowCount; ++ii)
			{
				ModelIndex childIndex = m_pModel->GetIndex(ii, 0, index);
				HideRowsRecursively(childIndex);
			}
		}

		pInfo->m_collapsed = !pInfo->m_collapsed;

		WidgetMgr::Get().RequestResize();
	}

	void TreeView::HeaderSplit_OnDrag(const Core::Int2& mousePosition, Label* pHeader, int columnIndex)
	{
		int labelRight = pHeader->GetScreenX() + pHeader->GetWidth();
		int offset = mousePosition.x - labelRight;
		int newWidth = pHeader->GetWidth() + offset;

		const int MIN_COLUMN_WIDTH = 10;
		if (newWidth < MIN_COLUMN_WIDTH)
			return;

		SetColumnWidth(columnIndex, newWidth);
		WidgetMgr::Get().RequestResize();
	}

	Widgets::Layout* TreeView::CreateItem(const ModelIndex& index, int columnCount, int depth)
	{
		Layout* pRowLayout = new Layout();
		pRowLayout->SetSpace(DirectX::XMINT2(5, 0));
		pRowLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
		pRowLayout->SetDirection(Layout::Horizontal);
		pRowLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pRowLayout->OnMouseDown([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, pRowLayout); });
		pRowLayout->OnMouseMove([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseMove_ItemLayout(ev, pRowLayout); });
		pRowLayout->OnMouseDoubleClick([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDoubleClick_ItemLayout(ev, pRowLayout); });
		pRowLayout->OnMouseUp([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseUp_ItemLayout(ev, pRowLayout); });

		RowInfo info;
		info.m_depth = depth;
		info.m_collapsed = false;
		info.m_pParent = ComputeRowLayoutFromModelIndex(index.GetParent());

		pRowLayout->GetDefaultStyle().SetBackgroundColor(m_defaultRowBackgroundColor);

		int childrenCount = index.GetConstModel()->GetRowCount(index);
		bool hasChildren = childrenCount > 0;

		for (int jj = 0; jj < columnCount; ++jj)
		{
			ModelIndex rowIndex = index.GetSiblingAtColumn(jj);
			if (!rowIndex.IsValid())
				continue;

			std::string data = m_pModel->GetData(rowIndex);

			Label* pLabel = new Label(data);
			Widget* pContainer = pLabel;
			if (jj == 0)
			{
				Layout* pExpandLayout = new Layout();
				pExpandLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
				pExpandLayout->SetDirection(Layout::Horizontal);
				pExpandLayout->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
				pExpandLayout->GetHoverStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));

				for (int ii = 0; ii <= depth-1; ++ii)
				{
					Container* pContainer = new Container(20, 1);
					pContainer->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
					pExpandLayout->AddWidget(pContainer);
				}

				Rendering::TextureId expandIcon = WidgetMgr::Get().GetIconTextureId(Widgets::IconId::kIconExpanded);
				Icon* pIcon = new Icon(expandIcon);
				pIcon->SetSize(Core::UInt2(20, 20));
				if (!hasChildren)
					pIcon->Hide();

				pIcon->OnMouseDown([this, pRowLayout](const Widgets::MouseEvent& ev) { OnClick_Icon(pRowLayout); });

				info.m_pIcon = pIcon;

				pExpandLayout->AddWidget(pIcon);
				pExpandLayout->AddWidget(pLabel);
				pContainer = pExpandLayout;
			}
			
			if (jj != columnCount - 1)
			{
				pContainer->SetSizeStyle(Widgets::Widget::DEFAULT);
				Core::UInt2 cellSize(m_columnWidth[jj], m_cellDefaultSize.y);
				pContainer->SetSize(cellSize);
			}
			else
			{
				pContainer->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			}

			pRowLayout->AddWidget(pContainer);
		}

		m_rowInfoMap[pRowLayout] = info;

		return pRowLayout;
	}

	Label* TreeView::GetCell(const ModelIndex& index)
	{
		Layout* pRowLayout = ComputeRowLayoutFromModelIndex(index);
		if (!pRowLayout)
			return nullptr;

		Widget* pCell = pRowLayout->GetChildren()[index.GetColumn()];
		if (!pCell)
			return nullptr;

		if (index.GetColumn() == 0) // first column is a layout, not a label
		{
			Layout* pCellLayout = static_cast<Layout*>(pCell);
			pCell = pCellLayout->GetChildren().back(); //the last widget is the label
		}

		Label* pLabel = static_cast<Label*>(pCell);
		return pLabel;
	}

	Layout* TreeView::ComputeRowLayoutFromModelIndex(const ModelIndex& index)
	{
		std::stack<ModelIndex> indexStack;

		ModelIndex temp = index;
		while (temp.IsValid())
		{
			indexStack.push(temp);
			temp = temp.GetParent();
		}

		Layout* pLayout = nullptr;
		int row = 0;
		while (!indexStack.empty())
		{
			temp = indexStack.top();
			indexStack.pop();

			std::map<const Layout*, Core::Array<Layout*>>::iterator it = m_rowLayoutTree.find(pLayout);
			if (it == m_rowLayoutTree.end())
				return nullptr;

			Core::Array<Layout*>& children = it->second;

			if (!children.IsValidIndex(temp.GetRow()))
				return nullptr;

			pLayout = children[temp.GetRow()];
		}

		return pLayout;
	}

	ModelIndex TreeView::ComputeModelIndexFromRowLayout(const Layout* pLayout)
	{
		std::stack<int> rowStack;

		const Layout* pChildLayout = pLayout;
		const RowInfo* pInfo = GetRowInfo(static_cast<const Widget*>(pChildLayout));
		while (pInfo)
		{
			Core::Array<Layout*>& childrenArray = m_rowLayoutTree[pInfo->m_pParent];
			Core::Array<Layout*>::Iterator it = std::find(childrenArray.cbegin(), childrenArray.cend(), pChildLayout);
			if (it == childrenArray.cend())
				return ModelIndex();

			int row = static_cast<int>(std::distance(childrenArray.cbegin(), it));
			rowStack.push(row);

			pChildLayout = pInfo->m_pParent;
			pInfo = GetRowInfo(static_cast<const Widget*>(pChildLayout));
		}

		ModelIndex index;
		while (!rowStack.empty())
		{
			index = m_pModel->GetIndex(rowStack.top(), 0, index);
			rowStack.pop();
		}

		return index;
	}

	int TreeView::GetRowIndexInLayout(const Layout* pLayout) const
	{
		const std::vector<Widgets::Widget*> children = m_pLayout->GetChildren();
		std::vector<Widgets::Widget*>::const_iterator it = std::find(children.cbegin(), children.cend(), pLayout);

		if (it == children.cend())
			return -1;

		int index = static_cast<int>(std::distance(children.cbegin(), it));
		return index;
	}

	TreeView::RowInfo* TreeView::GetRowInfo(const Widget* pRowLayout)
	{
		std::map<const Widget*, RowInfo>::iterator it = m_rowInfoMap.find(pRowLayout);
		if (it == m_rowInfoMap.cend())
			return nullptr;

		return &it->second;
	}

	void TreeView::HideRowsRecursively(const ModelIndex& indexToHide)
	{
		if(Layout* pLayout = ComputeRowLayoutFromModelIndex(indexToHide))
			pLayout->Disable();

		int rowCount = m_pModel->GetRowCount(indexToHide);
		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, indexToHide);
			HideRowsRecursively(childIndex);
		}
	}

	void TreeView::ShowRowsRecursively(const ModelIndex& indexToShow)
	{
		Layout* pLayout = ComputeRowLayoutFromModelIndex(indexToShow);
		if (!pLayout)
			return;

		pLayout->Enable();

		int rowCount = m_pModel->GetRowCount(indexToShow);
		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, indexToShow);


			RowInfo* pInfo = GetRowInfo(pLayout);
			if (!pInfo)
				continue;

			if (pInfo->m_collapsed)
				continue;

			ShowRowsRecursively(childIndex);
		}
	}

	void TreeView::DeleteRowRecursively(Layout* pParentRow, Layout* pRowToRemove)
	{
		//delete the children
		Core::Array<Layout*>& children = m_rowLayoutTree[pRowToRemove];
		for (Layout* pChild : children)
		{
			DeleteRowRecursively(pRowToRemove, pChild);
		}
		
		//delete the widgets
		m_pLayout->DeleteChild(pRowToRemove);

		//clean the caches
		m_rowInfoMap.erase(pRowToRemove);
		m_rowLayoutTree.erase(pRowToRemove);
		m_rowLayoutTree[pParentRow].Erase(pRowToRemove);
	}
}
