/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Entity/EntityWidget.h"

#include "Editors/Widgets/BaseModel.h"
#include "Editors/Widgets/Component/ComponentWidget.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	EntityWidget::EntityWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyWidget(true)
	{
		SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);
		SetSize(DirectX::XMUINT2(500, 0));
	}

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
		DeleteAllChildren();
		m_pModel = pModel;
		m_isDirtyWidget = true;
	}

	void EntityWidget::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pLayout->SetDirection(Widgets::Layout::Vertical);
		AddWidget(pLayout);

		const int FIELD_HEIGHT = 15;

		int rowCount = m_pModel->GetRowCount();
		for (int ii = 0; ii < rowCount; ++ii)
		{
			Widgets::Label* pNameLabel = new Widgets::Label();
			pNameLabel->SetText(m_pModel->GetData(ii, 0));
			pNameLabel->SetSize(DirectX::XMUINT2(0, FIELD_HEIGHT));
			pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pLayout->AddWidget(pNameLabel);

			ComponentWidget* pComponentWidget = new ComponentWidget();
			pComponentWidget->SetModel(m_pModel->GetSubModel(ii, 1));
			pLayout->AddWidget(pComponentWidget);
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}
}
