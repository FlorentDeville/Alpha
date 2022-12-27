/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Entity/EntityWidget.h"

#include "Editors/Widgets/BaseModel.h"
#include "Editors/Widgets/Component/ComponentWidget.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"

namespace Editors
{
	EntityWidget::EntityWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyWidget(true)
	{}

	EntityWidget::~EntityWidget()
	{}

	void EntityWidget::Update()
	{
		if (m_isDirtyWidget)
		{
			CreateWidgets();
			m_isDirtyWidget = false;
		}
	}

	void EntityWidget::SetModel(const BaseModel* pModel)
	{
		m_pModel = pModel;
	}

	void EntityWidget::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pLayout->SetDirection(Widgets::Layout::Vertical);
		AddWidget(pLayout);

		int rowCount = m_pModel->GetRowCount();
		for (int ii = 0; ii < rowCount; ++ii)
		{
			Widgets::Label* pNameLabel = new Widgets::Label();
			pNameLabel->SetText(m_pModel->GetData(ii, 0));
			pNameLabel->SetSize(DirectX::XMUINT2(100, 15));
			pNameLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			pLayout->AddWidget(pNameLabel);

			ComponentWidget* pComponentWidget = new ComponentWidget();
			pComponentWidget->SetModel(m_pModel->GetSubModel(ii, 1));
			pComponentWidget->SetSize(DirectX::XMUINT2(500, 5 * 15));
			pLayout->AddWidget(pComponentWidget);
		}

		ResizeChildren();
	}
}
