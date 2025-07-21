/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TreeView.h"

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
	{
		SetSizeStyle(Widgets::Widget::STRETCH);

		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(SIZE_STYLE::STRETCH);
		m_pLayout->SetDirection(Layout::Vertical);
		m_pLayout->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
		m_pLayout->GetHoverStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f));
		AddWidget(m_pLayout);
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
		m_pModel->OnRemoveRows([this](int start, int count, const ModelIndex& parent) { OnRemoveRows(start, count, parent); });
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
		if (!m_pModel)
			return;

		int columnCount = m_pModel->GetColumnCount(Widgets::ModelIndex());
		if (column < 0 || column >= columnCount)
			return;

		if (m_columnWidth.GetSize() < static_cast<uint32_t>(columnCount))
			m_columnWidth.Resize(columnCount, m_cellDefaultSize.x);

		m_columnWidth[column] = width;

		std::vector<Widgets::Widget*> rows = m_pLayout->GetChildren();

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

		CreateView_Recursive(root, 0);
	}

	void TreeView::CreateView_Recursive(const ModelIndex& parent, int depth)
	{
		int rowCount = m_pModel->GetRowCount(parent);
		int columnCount = m_pModel->GetColumnCount(parent);

		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, parent);
			int childrenCount = m_pModel->GetRowCount(childIndex);
			bool hasChildren = childrenCount > 0;

			Layout* pRowLayout = CreateItem(ii, columnCount, parent, depth, hasChildren);
			m_pLayout->AddWidget(pRowLayout);

			
			CreateView_Recursive(childIndex, depth + 1);
		}
	}

	void TreeView::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		std::map<Widget*, RowInfo>::const_iterator it = m_rowInfoMap.find(pRowLayout);
		if (it == m_rowInfoMap.cend())
			return;

		ModelIndex start = it->second.m_index;
		ModelIndex end = m_pModel->GetIndex(start.GetRow(), m_pModel->GetColumnCount(ModelIndex()) - 1, start.GetParent());
		SelectionRow clickedRow(start, end);

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		bool rowSelected = pSelectionModel->IsRowSelected(clickedRow);

		if (rowSelected)
		{
			//deselect
			SetDeselectedRowStyle(pRowLayout);
			pSelectionModel->DeselectRow(clickedRow);
		}
		else
		{
			//select
			if (m_multiSelectionEnabled)
			{
				SetSelectedRowStyle(pRowLayout);
				pSelectionModel->SelectRow(clickedRow);
			}
			else
			{
				const std::list<SelectionRow>& selection = pSelectionModel->GetSelectedRows();
				for (const SelectionRow& sel : selection)
				{
					std::map<ModelIndex, Layout*>::const_iterator it = m_modelIndexToRowMap.find(sel.GetStartIndex());
					if(it != m_modelIndexToRowMap.cend())
						SetDeselectedRowStyle(it->second);
				}
				SetSelectedRowStyle(pRowLayout);
				pSelectionModel->SetSelectionRow(clickedRow);
			}
		}
	}

	void TreeView::OnMouseDoubleClick_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		int row = GetRowIndex(pRowLayout);
		if (row < 0)
			return;

		ModelIndex idx = m_pModel->GetIndex(row, 0, ModelIndex());
		m_onItemDoubleClick(idx);
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
		//This is a table, only root can have children
		if (parent.IsValid())
			return;

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
			int rowCount = m_pModel->GetRowCount(newIndex);
			bool hasChildren = rowCount > 0;

			Layout* pRowLayout = CreateItem(ii, columnCount, root, depth, hasChildren);

			// I need to calculate the correct position for this row in the layout.
			// It needs to go after all the children of the previous sibling. It's easier to do
			// the reverse and to add it before the next sibling of my parent
			//int layoutIndex = GetRowLayoutIndex(ii);

			Widgets::ModelIndex followingSibling;
			Widgets::ModelIndex tempParentIndex = parent;
			while (tempParentIndex.IsValid())
			{
				Widgets::ModelIndex grandParent = tempParentIndex.GetParent();
				followingSibling = m_pModel->GetIndex(tempParentIndex.GetRow(), 0, grandParent);
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
				std::map<ModelIndex, Layout*>::iterator it = m_modelIndexToRowMap.find(followingSibling);
				if (it == m_modelIndexToRowMap.end())
				{
					const std::vector<Widget*>& allChildren = m_pLayout->GetChildren();
					std::vector<Widget*>::const_iterator itWidget = std::find(allChildren.cbegin(), allChildren.cend(), it->second);
					if (itWidget != allChildren.cend())
					{
						size_t layoutIndex = std::distance(allChildren.cbegin(), itWidget);
						m_pLayout->InsertWidget(pRowLayout, static_cast<int>(layoutIndex));
					}
				}
			}
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TreeView::OnRemoveRows(int start, int count, const ModelIndex& parent)
	{
		if (parent.IsValid())
			return;

		Layout* pLayout = GetRowWidget(start, 0, parent);
		if (!pLayout)
			return;

		m_pLayout->DeleteChild(pLayout);

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
			std::map<ModelIndex, Layout*>::const_iterator it = m_modelIndexToRowMap.find(deselectedRow.GetStartIndex());
			if (it != m_modelIndexToRowMap.cend())
				SetDeselectedRowStyle(it->second);
		}

		for (const SelectionRow& selectedRow : selected)
		{
			std::map<ModelIndex, Layout*>::const_iterator it = m_modelIndexToRowMap.find(selectedRow.GetStartIndex());
			if (it != m_modelIndexToRowMap.cend())
				SetSelectedRowStyle(it->second);
		}
	}

	void TreeView::OnClick_Icon(Layout* pRowLayout)
	{
		std::map<Widget*, RowInfo>::iterator it = m_rowInfoMap.find(pRowLayout);
		if (it == m_rowInfoMap.cend())
			return;

		RowInfo& info = it->second;

		if (info.m_collapsed)
		{
			Rendering::TextureId expandedIcon = WidgetMgr::Get().GetIconTextureId(Widgets::IconId::kIconExpanded);
			info.m_pIcon->SetTextureId(expandedIcon);

			int rowCount = m_pModel->GetRowCount(info.m_index);
			for (int ii = 0; ii < rowCount; ++ii)
			{
				ModelIndex childIndex = m_pModel->GetIndex(ii, 0, info.m_index);
				ShowRowsRecursively(childIndex);
			}
		}
		else
		{
			Rendering::TextureId collapsedIcon = WidgetMgr::Get().GetIconTextureId(Widgets::IconId::kIconCollapsed);
			info.m_pIcon->SetTextureId(collapsedIcon);

			int rowCount = m_pModel->GetRowCount(info.m_index);
			for (int ii = 0; ii < rowCount; ++ii)
			{
				ModelIndex childIndex = m_pModel->GetIndex(ii, 0, info.m_index);
				HideRowsRecursively(childIndex);
			}
		}

		info.m_collapsed = !info.m_collapsed;

		WidgetMgr::Get().RequestResize();
	}

	Widgets::Layout* TreeView::CreateItem(int row, int columnCount, const ModelIndex& parent, int depth, bool hasChildren)
	{
		Layout* pRowLayout = new Layout();
		pRowLayout->SetSpace(DirectX::XMINT2(5, 0));
		pRowLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
		pRowLayout->SetDirection(Layout::Horizontal);
		pRowLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pRowLayout->OnMouseDown([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, pRowLayout); });
		pRowLayout->OnMouseDoubleClick([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDoubleClick_ItemLayout(ev, pRowLayout); });

		RowInfo info;
		info.m_depth = depth;
		info.m_index = m_pModel->GetIndex(row, 0, parent);
		info.m_collapsed = false;
		
		m_modelIndexToRowMap[info.m_index] = pRowLayout;

		pRowLayout->GetDefaultStyle().SetBackgroundColor(m_defaultRowBackgroundColor);

		for (int jj = 0; jj < columnCount; ++jj)
		{
			ModelIndex rowIndex = m_pModel->GetIndex(row, jj, parent);
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
		std::map<ModelIndex, Layout*>::iterator it = m_modelIndexToRowMap.find(index);
		if (it == m_modelIndexToRowMap.end())
			return nullptr;

		Layout* pRowLayout = it->second;
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

	Layout* TreeView::GetRowWidget(int row, int column, const ModelIndex& parent)
	{
		if (parent.IsValid())
			return nullptr;

		int layoutIndex = GetRowLayoutIndex(row);
		Widget* pRow = m_pLayout->GetChildren()[layoutIndex]; //+1 because the first row is the header
		if (!pRow)
			return nullptr;

		Layout* pRowLayout = static_cast<Layout*>(pRow);
		return pRowLayout;
	}

	int TreeView::GetRowIndex(const Layout* pLayout) const
	{
		const std::vector<Widgets::Widget*> children = m_pLayout->GetChildren();
		std::vector<Widgets::Widget*>::const_iterator it = std::find(children.cbegin(), children.cend(), pLayout);

		if (it == children.cend())
			return -1;

		int index = static_cast<int>(std::distance(children.cbegin(), it));
		index = index - 1; //-1 because the first row is the header
		return index;
	}

	int TreeView::GetRowLayoutIndex(int row) const
	{
		return row + 1; //+1 because the first row is the header.
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

	void TreeView::HideRowsRecursively(const ModelIndex& indexToHide)
	{
		std::map<ModelIndex, Layout*>::iterator it = m_modelIndexToRowMap.find(indexToHide);
		if (it != m_modelIndexToRowMap.end())
		{
			it->second->Disable();
		}

		int rowCount = m_pModel->GetRowCount(indexToHide);
		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, indexToHide);
			HideRowsRecursively(childIndex);
		}
	}

	void TreeView::ShowRowsRecursively(const ModelIndex& indexToShow)
	{
		std::map<ModelIndex, Layout*>::iterator it = m_modelIndexToRowMap.find(indexToShow);
		if (it != m_modelIndexToRowMap.end())
		{
			it->second->Enable();
		}

		int rowCount = m_pModel->GetRowCount(indexToShow);
		for (int ii = 0; ii < rowCount; ++ii)
		{
			ModelIndex childIndex = m_pModel->GetIndex(ii, 0, indexToShow);

			//2 lookups, aouch. I should have map Layout* -> RowInfo*
			std::map<ModelIndex, Layout*>::iterator it = m_modelIndexToRowMap.find(indexToShow);
			if (it == m_modelIndexToRowMap.end())
				continue;

			std::map<Widget*, RowInfo>::iterator itInfo = m_rowInfoMap.find(it->second);
			if (itInfo == m_rowInfoMap.end())
				continue;

			if (itInfo->second.m_collapsed)
				continue;

			ShowRowsRecursively(childIndex);
		}
	}
}
