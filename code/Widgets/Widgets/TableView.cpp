/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TableView.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"

#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/Widgets/TableViewStyle.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Split.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	TableView::TableView()
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
		m_pLayout->GetDefaultStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f)); //transparent
		m_pLayout->GetHoverStyle().SetBackgroundColor(Color(0.f, 0.f, 0.f, 0.f)); //transparent
		AddWidget(m_pLayout);
	}

	TableView::~TableView()
	{
		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}
	}

	void TableView::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
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

	void TableView::SetModel(AbstractViewModel* pModel)
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
		m_pModel->OnDataChanged([this](const ModelIndex& index) { OnDataChanged_SelectionModel(index); });

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected) { OnSelectionChanged_SelectionModel(selected, deselected); });

		m_columnWidth.Resize(static_cast<uint32_t>(m_pModel->GetColumnCount(Widgets::ModelIndex())), m_cellDefaultSize.x);

		CreateHeader();
		CreateView();
	}

	void TableView::SetMultiSelection(bool enable)
	{
		m_multiSelectionEnabled = enable;
	}

	void TableView::SetColumnWidth(int column, int width)
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

	void TableView::CreateHeader()
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

	void TableView::CreateView()
	{
		ModelIndex root;

		int rowCount = m_pModel->GetRowCount(root);
		int columnCount = m_pModel->GetColumnCount(root);

		for (int ii = 0; ii < rowCount; ++ii)
		{
			Layout* pRowLayout = CreateItem(ii, columnCount, root);
			m_pLayout->AddWidget(pRowLayout);
		}
	}

	void TableView::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		int row = GetRowIndex(pRowLayout);
		if (row < 0)
			return;

		ModelIndex start = m_pModel->GetIndex(row, 0, ModelIndex());
		ModelIndex end = m_pModel->GetIndex(row, m_pModel->GetColumnCount(ModelIndex()) - 1, ModelIndex());
		SelectionRow clickedRow(start, end);

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		bool rowSelected = pSelectionModel->IsRowSelected(clickedRow);

		if (rowSelected)
		{
			//deselect
			SetDeselectedRowStyle(row);
			pSelectionModel->DeselectRow(clickedRow);
		}
		else
		{
			//select
			if (m_multiSelectionEnabled)
			{
				SetSelectedRowStyle(row);
				pSelectionModel->SelectRow(clickedRow);
			}
			else
			{
				const std::list<SelectionRow>& selection = pSelectionModel->GetSelectedRows();
				for (const SelectionRow& sel : selection)
					SetDeselectedRowStyle(sel.GetStartIndex().GetRow());

				SetSelectedRowStyle(row);
				pSelectionModel->SetSelectionRow(clickedRow);
			}
		}
	}

	void TableView::OnMouseDoubleClick_ItemLayout(const Widgets::MouseEvent& ev, Widgets::Layout* pRowLayout)
	{
		int row = GetRowIndex(pRowLayout);
		if (row < 0)
			return;

		ModelIndex idx = m_pModel->GetIndex(row, 0, ModelIndex());
		m_onItemDoubleClick(idx);
	}

	void TableView::SetSelectedRowStyle(int row)
	{
		Widgets::Layout* pLayout = GetRowWidget(row, 0, Widgets::ModelIndex());
		pLayout->GetDefaultStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderColor(m_selectedBorderColor);

		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBorderColor(m_selectedBorderColor);
	}

	void TableView::SetDeselectedRowStyle(int row)
	{
		Layout* pLayout = GetRowWidget(row, 0, Widgets::ModelIndex());

		pLayout->GetDefaultStyle().SetBackgroundColor(m_defaultRowBackgroundColor);
		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(false);
		pLayout->GetDefaultStyle().ShowBorder(false);
	}

	void TableView::OnCommitInsertRows(int start, int count, const ModelIndex& parent)
	{
		//This is a table, only root can have children
		if (parent.IsValid())
			return;

		ModelIndex root;
		int columnCount = m_pModel->GetColumnCount(root);
		
		//handle selection!!!!

		for (int ii = start; ii < start + count; ++ii)
		{
			Layout* pRowLayout = CreateItem(ii, columnCount, root);

			int layoutIndex = GetRowLayoutIndex(ii);
			m_pLayout->InsertWidget(pRowLayout, layoutIndex);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TableView::OnRemoveRows(int start, int count, const ModelIndex& parent)
	{
		if (parent.IsValid())
			return;

		Layout* pLayout = GetRowWidget(start, 0, parent);
		if (!pLayout)
			return;

		m_pLayout->DeleteChild(pLayout);

		WidgetMgr::Get().RequestResize();
	}

	void TableView::OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected)
	{
		for (const SelectionRow& deselectedRow : deselected)
		{
			//if the row was removed, it's possible deselected contains an out of range index
			size_t childrenCount = m_pLayout->GetChildren().size();
			int row = deselectedRow.GetStartIndex().GetRow();
			
			if (row < 0 || row >= childrenCount)
				continue;

			SetDeselectedRowStyle(deselectedRow.GetStartIndex().GetRow());
		}

		for (const SelectionRow& selectedRow : selected)
		{
			SetSelectedRowStyle(selectedRow.GetStartIndex().GetRow());
		}
	}

	void TableView::OnDataChanged_SelectionModel(const ModelIndex& index)
	{
		Label* pLabel = GetItem(index.GetRow(), index.GetColumn(), index.GetParent());
		if (!pLabel)
			return;

		std::string value = m_pModel->GetData(index);
		pLabel->SetText(value);
	}

	Widgets::Layout* TableView::CreateItem(int row, int columnCount, const ModelIndex& parent)
	{
		Layout* pRowLayout = new Layout();
		pRowLayout->SetSpace(DirectX::XMINT2(5, 0));
		pRowLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
		pRowLayout->SetDirection(Layout::Horizontal);
		pRowLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pRowLayout->OnMouseDown([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, pRowLayout); });
		pRowLayout->OnMouseDoubleClick([this, pRowLayout](const Widgets::MouseEvent& ev) { OnMouseDoubleClick_ItemLayout(ev, pRowLayout); });
		pRowLayout->GetDefaultStyle().SetBackgroundColor(m_defaultRowBackgroundColor);

		for (int jj = 0; jj < columnCount; ++jj)
		{
			ModelIndex rowIndex = m_pModel->GetIndex(row, jj, parent);
			if (!rowIndex.IsValid())
				continue;

			std::string data = m_pModel->GetData(rowIndex);

			Label* pLabel = new Label(data);
			if (jj != columnCount - 1)
			{
				pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
				Core::UInt2 cellSize(m_columnWidth[jj], m_cellDefaultSize.y);
				pLabel->SetSize(cellSize);
			}
			else
			{
				pLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			}

			pRowLayout->AddWidget(pLabel);
		}

		return pRowLayout;
	}

	Label* TableView::GetItem(int row, int column, const ModelIndex& parent)
	{
		Layout* pRowLayout = GetRowWidget(row, column, parent);
		if (!pRowLayout)
			return nullptr;

		Widget* pCell = pRowLayout->GetChildren()[column];
		if (!pCell)
			return nullptr;

		Label* pLabel = static_cast<Label*>(pCell);
		return pLabel;
	}

	Layout* TableView::GetRowWidget(int row, int column, const ModelIndex& parent)
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

	int TableView::GetRowIndex(const Layout* pLayout) const
	{
		const std::vector<Widgets::Widget*> children = m_pLayout->GetChildren();
		std::vector<Widgets::Widget*>::const_iterator it = std::find(children.cbegin(), children.cend(), pLayout);

		if (it == children.cend())
			return -1;

		int index = static_cast<int>(std::distance(children.cbegin(), it));
		index = index - 1; //-1 because the first row is the header
		return index;
	}

	int TableView::GetRowLayoutIndex(int row) const
	{
		return row + 1; //+1 because the first row is the header.
	}

	void TableView::HeaderSplit_OnDrag(const Core::Int2& mousePosition, Label* pHeader, int columnIndex)
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
}
