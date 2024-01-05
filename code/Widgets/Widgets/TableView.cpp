/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TableView.h"

#include "Widgets/Events/MouseEvent.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRange.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Widgets
{
	TableView::TableView()
		: Widget()
		, m_pModel(nullptr)
		, m_oddRowBackgroundColor(0.12f, 0.12f, 0.12f, 1.f)
		, m_evenRowBackgroundColor(0.16f, 0.16f, 0.16f, 1.f)
		, m_hoverBackgroundColor(0.24f, 0.24f, 0.24f, 1.f)
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
		CreateView();
	}

	void TableView::CreateView()
	{
		ModelIndex root;

		int rowCount = m_pModel->GetRowCount(root);
		int columnCount = m_pModel->GetColumnCount(root);

		for (int ii = 0; ii < rowCount; ++ii)
		{
			Layout* pRowLayout = new Layout();
			pRowLayout->SetSpace(DirectX::XMINT2(5, 0));
			pRowLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
			pRowLayout->SetDirection(Layout::Horizontal);
			pRowLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
			pRowLayout->OnMouseDown([this, ii](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, ii); });

			if (ii % 2 == 0)
			{
				pRowLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
			}

			m_pLayout->AddWidget(pRowLayout);

			for (int jj = 0; jj < columnCount; ++jj)
			{
				ModelIndex rowIndex = m_pModel->GetIndex(ii, jj, root);
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
		}
	}

	void TableView::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, int row)
	{
		if (!ev.HasButton(MouseButton::LeftButton))
			return;

		ModelIndex start = m_pModel->GetIndex(row, 0, ModelIndex());
		ModelIndex end = m_pModel->GetIndex(row, m_pModel->GetColumnCount(ModelIndex()) - 1, ModelIndex());
		SelectionRange range(start, end);

		SelectionModel* pSelectionModel = m_pModel->GetSelectionModel();
		bool rowSelected = pSelectionModel->IsRowSelected(range);

		if (rowSelected)
		{
			//deselect

			Widget* pWidget = m_pLayout->GetChildren()[row];
			Layout* pLayout = static_cast<Layout*>(pWidget);

			if (row % 2 == 0)
				pLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
			else
				pLayout->GetDefaultStyle().SetBackgroundColor(m_oddRowBackgroundColor);

			pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
			pLayout->GetHoverStyle().ShowBorder(false);
			pLayout->GetDefaultStyle().ShowBorder(false);
			
			pSelectionModel->DeselectRow(range);
		}
		else
		{
			//select		
			Widgets::Layout* pLayout = static_cast<Widgets::Layout*>(m_pLayout->GetChildren()[row]);
			pLayout->GetDefaultStyle().SetBackgroundColor(m_hoverBackgroundColor);
			pLayout->GetDefaultStyle().ShowBorder(true);
			pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
			pLayout->GetHoverStyle().ShowBorder(true);
			pSelectionModel->SelectRow(range);
		}
	}
}
