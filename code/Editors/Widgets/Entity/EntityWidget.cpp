/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Entity/EntityWidget.h"

#include "Editors/Widgets/BaseModel.h"
#include "Editors/Widgets/Component/ComponentWidget.h"

#include "Widgets/Container.h"
#include "Widgets/Icon.h"
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
			Widgets::Layout* pComponentLayout = new Widgets::Layout();
			pComponentLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pComponentLayout->SetDirection(Widgets::Layout::Vertical);
			pComponentLayout->GetDefaultStyle().ShowBorder(true);
			pComponentLayout->GetDefaultStyle().SetBorderSize(1);
			pLayout->AddWidget(pComponentLayout);

			Widgets::Layout* pNameLayout = new Widgets::Layout();
			pNameLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pNameLayout->SetDirection(Widgets::Layout::Horizontal);

			std::string iconPath = Widgets::WidgetMgr::Get().GetEditorIconsPath() + "\\collapsed.png";
			Widgets::Icon* pIcon = new Widgets::Icon(DirectX::XMINT2(0, 0), DirectX::XMUINT2(12, 12), iconPath);
			pIcon->SetY(3);
			
			pNameLayout->AddWidget(pIcon);

			Widgets::Label* pNameLabel = new Widgets::Label();

			pNameLabel->SetText(m_pModel->GetData(ii, 0));
			pNameLabel->SetSize(DirectX::XMUINT2(0, FIELD_HEIGHT));
			pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pNameLayout->AddWidget(pNameLabel);
			
			pComponentLayout->AddWidget(pNameLayout);

			ComponentWidget* pComponentWidget = new ComponentWidget();
			pComponentWidget->SetModel(m_pModel->GetSubModel(ii, 1));

			pComponentLayout->AddWidget(pComponentWidget);

			Widgets::Container* pContainer = new Widgets::Container(10, 5);

			pLayout->AddWidget(pContainer);

			pIcon->OnClick([pComponentWidget](int, int) -> bool {
				pComponentWidget->IsEnabled() ? pComponentWidget->Disable() : pComponentWidget->Enable();
				Widgets::WidgetMgr::Get().RequestResize();
				return true;
				});
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}
}
