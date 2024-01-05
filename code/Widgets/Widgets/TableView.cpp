/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TableView.h"

#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/Modelndex.h"

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
}
