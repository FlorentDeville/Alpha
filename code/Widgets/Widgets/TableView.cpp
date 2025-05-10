/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TableView.h"

#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	TableView::TableView()
		: Widget()
		, m_pModel(nullptr)
		, m_oddRowBackgroundColor(0.12f, 0.12f, 0.12f, 1.f)
		, m_evenRowBackgroundColor(0.16f, 0.16f, 0.16f, 1.f)
		, m_hoverBackgroundColor(0.24f, 0.24f, 0.24f, 1.f)
		, m_selectedBorderColor(0.44f, 0.44f, 0.44f, 1.f)
		, m_multiSelectionEnabled(false)
	{
		m_pLayout = new Layout();
		m_pLayout->SetSizeStyle(SIZE_STYLE::STRETCH);
		m_pLayout->SetDirection(Layout::Vertical);

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
		m_pModel->OnDataChanged([this](const ModelIndex& index) { OnDataChanged_SelectionModel(index); });

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		pSelectionModel->OnSelectionChanged([this](const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected) { OnSelectionChanged_SelectionModel(selected, deselected); });

		CreateView();
	}

	void TableView::SetMultiSelection(bool enable)
	{
		m_multiSelectionEnabled = enable;
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

	void TableView::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, int row)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		ModelIndex start = m_pModel->GetIndex(row, 0, ModelIndex());
		ModelIndex end = m_pModel->GetIndex(row, m_pModel->GetColumnCount(ModelIndex()), ModelIndex());
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

	void TableView::OnMouseDoubleClick_ItemLayout(const Widgets::MouseEvent& ev, int row)
	{
		ModelIndex idx = m_pModel->GetIndex(row, 0, ModelIndex());
		m_onItemDoubleClick(idx);
	}

	void TableView::SetSelectedRowStyle(int row)
	{
		Widgets::Layout* pLayout = static_cast<Widgets::Layout*>(m_pLayout->GetChildren()[row]);
		pLayout->GetDefaultStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetDefaultStyle().SetBorderColor(m_selectedBorderColor);

		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBorderColor(m_selectedBorderColor);
	}

	void TableView::SetDeselectedRowStyle(int row)
	{
		Widget* pWidget = m_pLayout->GetChildren()[row];
		Layout* pLayout = static_cast<Layout*>(pWidget);

		if (row % 2 == 0)
			pLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
		else
			pLayout->GetDefaultStyle().SetBackgroundColor(m_oddRowBackgroundColor);

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
			m_pLayout->InsertWidget(pRowLayout, ii);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void TableView::OnSelectionChanged_SelectionModel(const std::vector<SelectionRow>& selected, const std::vector<SelectionRow>& deselected)
	{
		for (const SelectionRow& deselectedRow : deselected)
		{
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
		pRowLayout->OnMouseDown([this, row](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, row); });
		pRowLayout->OnMouseDoubleClick([this, row](const Widgets::MouseEvent& ev) { OnMouseDoubleClick_ItemLayout(ev, row); });

		if (row % 2 == 0)
		{
			pRowLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
		}

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
				pLabel->SetSize(DirectX::XMUINT2(75, 20));
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
		if (parent.IsValid())
			return nullptr;

		Widget* pRow = m_pLayout->GetChildren()[row];
		if (!pRow)
			return nullptr;

		Layout* pRowLayout = static_cast<Layout*>(pRow);
		
		Widget* pCell = pRowLayout->GetChildren()[column];
		if (!pCell)
			return nullptr;

		Label* pLabel = static_cast<Label*>(pCell);
		return pLabel;
	}
}
