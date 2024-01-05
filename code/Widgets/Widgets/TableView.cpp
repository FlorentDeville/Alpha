/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/TableView.h"

#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/Modelndex.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"

namespace Widgets
{
	TableView::TableView()
		: Widget()
		, m_pModel(nullptr)
		, m_columnCount(1)
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

	void TableView::SetColumnCount(int columnCount)
	{
		m_columnCount = columnCount;
	}

	void TableView::CreateView()
	{
		ModelIndex root;

		int rowCount = m_pModel->GetRowCount(root);

		for (int ii = 0; ii < rowCount; ++ii)
		{
			Layout* pRowLayout = new Layout();
			pRowLayout->SetSizeStyle(SIZE_STYLE::HSIZE_STRETCH | SIZE_STYLE::VSIZE_FIT);
			pRowLayout->SetDirection(Layout::Horizontal);

			m_pLayout->AddWidget(pRowLayout);

			for (int jj = 0; jj < m_columnCount; ++jj)
			{
				ModelIndex rowIndex = m_pModel->GetIndex(ii, jj, root);
				if (!rowIndex.IsValid())
					continue;

				std::string data = m_pModel->GetData(rowIndex);

				Button* pButton = new Button(data);
				pRowLayout->AddWidget(pButton);
			}
		}
	}
}
