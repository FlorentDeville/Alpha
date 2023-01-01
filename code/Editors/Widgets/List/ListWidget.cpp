/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/List/ListWidget.h"

#include "Editors/Widgets/BaseModel.h"

//#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	ListWidget::ListWidget()
		: Widgets::Widget()
		, m_isDirtyWidget(false)
		, m_pModel(nullptr)
	{
		SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);

		m_pArrayLayout = new Widgets::Layout();
		m_pArrayLayout->SetDirection(Widgets::Layout::Vertical);
		m_pArrayLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		AddWidget(m_pArrayLayout);
	}

	ListWidget::~ListWidget()
	{}

	void ListWidget::Update(uint64_t dt)
	{
		if (m_isDirtyWidget && m_pModel)
		{
			m_pArrayLayout->DeleteAllChildren();
			
			const int ITEM_HEIGHT = 20;

			const int rowCount = m_pModel->GetRowCount();
			const int columnCount = m_pModel->GetColumnCount();
			for (int ii = 0; ii < rowCount; ++ii)
			{
				Widgets::Layout* pItemLayout = new Widgets::Layout();
				pItemLayout->SetSize(DirectX::XMUINT2(0, 20));
				pItemLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
				pItemLayout->SetDirection(Widgets::Layout::Horizontal);
				pItemLayout->GetHoverStyle().SetBackgroundColor(DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f));

				if (ii % 2 == 0)
				{
					pItemLayout->GetDefaultStyle().SetBackgroundColor(DirectX::XMVectorSet(0.16f, 0.16f, 0.16f, 1.f));
				}

				m_pArrayLayout->AddWidget(pItemLayout);

				for (int jj = 0; jj < columnCount; ++jj)
				{
					Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, m_pModel->GetData(ii, jj));
					pItemLayout->AddWidget(pLabel);

					int columnSize = 75;
					if (jj < m_columnSize.size())
						columnSize = m_columnSize[jj];

					pLabel->SetSize(DirectX::XMUINT2(columnSize, 20));

					if (jj != columnCount - 1)
					{
						pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
					}
					else
					{
						pLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
					}
				}
			}

			m_isDirtyWidget = false;
		}
	}

	void ListWidget::SetModel(BaseModel* pModel)
	{
		m_pModel = pModel;
		m_isDirtyWidget = true;
	}

	void ListWidget::SetColumnSize(int columnIndex, int size)
	{
		if (columnIndex >= m_columnSize.size())
			m_columnSize.resize(columnIndex + 1);

		m_columnSize[columnIndex] = size;
	}
}
