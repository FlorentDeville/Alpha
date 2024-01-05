/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/List/ListWidget.h"

#include "Editors/Widgets/BaseModel.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	ListWidget::ListWidget()
		: Widgets::Widget()
		, m_isDirtyWidget(false)
		, m_pModel(nullptr)
		, m_selectedRow(-1)
		, m_oddRowBackgroundColor(0.12f, 0.12f, 0.12f, 1.f)
		, m_evenRowBackgroundColor(0.16f, 0.16f, 0.16f, 1.f)
		, m_hoverBackgroundColor(0.24f, 0.24f, 0.24f, 1.f)
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
				pItemLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
				pItemLayout->OnMouseDown([this, ii](const Widgets::MouseEvent& ev) { OnMouseDown_ItemLayout(ev, ii); });
				pItemLayout->OnMouseDoubleClick([this, ii](const Widgets::MouseEvent& ev) { m_onItemDoubleClick(ii); });

				if (ii % 2 == 0)
				{
					pItemLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
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
			Widgets::WidgetMgr::Get().RequestResize();
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

	int ListWidget::GetSelectedItem() const
	{
		return m_selectedRow;
	}

	void ListWidget::OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, int itemIndex)
	{
		if (!ev.HasButton(Widgets::MouseButton::LeftButton))
			return;

		if (m_selectedRow != -1)
		{
			Widgets::Layout* pLayout = static_cast<Widgets::Layout*>(m_pArrayLayout->GetChildren()[m_selectedRow]);
			if (m_selectedRow % 2 == 0)
				pLayout->GetDefaultStyle().SetBackgroundColor(m_evenRowBackgroundColor);
			else
				pLayout->GetDefaultStyle().SetBackgroundColor(m_oddRowBackgroundColor);

			pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
			pLayout->GetHoverStyle().ShowBorder(false);
			pLayout->GetDefaultStyle().ShowBorder(false);

		}

		m_selectedRow = itemIndex;
		Widgets::Layout* pLayout = static_cast<Widgets::Layout*>(m_pArrayLayout->GetChildren()[itemIndex]);
		pLayout->GetDefaultStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetDefaultStyle().ShowBorder(true);
		pLayout->GetHoverStyle().SetBackgroundColor(m_hoverBackgroundColor);
		pLayout->GetHoverStyle().ShowBorder(true);
	}
}
