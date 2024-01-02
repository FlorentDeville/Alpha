/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Entity/EntityWidget.h"

#include "Editors/Widgets/Component/ComponentModel.h"
#include "Editors/Widgets/Component/ComponentWidget.h"
#include "Editors/Widgets/Entity/EntityModel.h"

#include "Rendering/Texture/TextureMgr.h"
#include "Rendering/Texture/Texture.h"

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
		SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);

		m_collapsedIcon = Widgets::WidgetMgr::Get().GetCollapsedIcon();
		m_expandedIcon = Widgets::WidgetMgr::Get().GetExpandedIcon();
	}

	EntityWidget::~EntityWidget()
	{
		delete m_pModel;
	}

	void EntityWidget::Update(uint64_t dt)
	{
		if (m_isDirtyWidget)
		{
			CreateWidgets();
			m_isDirtyWidget = false;
		}
	}

	void EntityWidget::SetModel(EntityModel* pModel)
	{
		if (m_pModel == pModel)
			return;

		if (m_pModel)
			delete m_pModel;

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

		int rowCount = m_pModel->GetComponentCount();
		for (int ii = 0; ii < rowCount; ++ii)
		{
			Widgets::Layout* pComponentLayout = new Widgets::Layout();
			pComponentLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pComponentLayout->SetDirection(Widgets::Layout::Vertical);
			pComponentLayout->GetDefaultStyle().ShowBorder(true);
			pComponentLayout->GetDefaultStyle().SetBorderSize(1);
			pComponentLayout->GetHoverStyle().ShowBorder(true);
			pComponentLayout->GetHoverStyle().SetBorderSize(1);
			pLayout->AddWidget(pComponentLayout);

			Widgets::Layout* pNameLayout = new Widgets::Layout();
			pNameLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pNameLayout->SetDirection(Widgets::Layout::Horizontal);

			Widgets::Icon* pIcon = new Widgets::Icon();
			pIcon->SetSize(DirectX::XMUINT2(12, 12));
			pIcon->SetTextureId(m_expandedIcon);
			pIcon->SetY(3);
			
			pNameLayout->AddWidget(pIcon);

			Widgets::Label* pNameLabel = new Widgets::Label();

			pNameLabel->SetText(m_pModel->GetComponentName(ii));
			pNameLabel->SetSize(DirectX::XMUINT2(0, FIELD_HEIGHT + 3));
			pNameLabel->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pNameLayout->AddWidget(pNameLabel);
			
			pComponentLayout->AddWidget(pNameLayout);

			ComponentWidget* pComponentWidget = new ComponentWidget();
			pComponentWidget->SetModel(m_pModel->GetComponentModel(ii));

			pComponentLayout->AddWidget(pComponentWidget);

			Widgets::Container* pContainer = new Widgets::Container(10, 5);

			pLayout->AddWidget(pContainer);

			pIcon->OnMouseDown([this, pComponentWidget, pIcon](const Widgets::MouseEvent& ev) { OnMouseDown_ExpandIcon(ev, pIcon, pComponentWidget); });
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}

	void EntityWidget::OnMouseDown_ExpandIcon(const Widgets::MouseEvent& ev, Widgets::Icon* pIcon, ComponentWidget* pComponentWidget)
	{
		if (!ev.HasButton(Widgets::MouseButton::LeftButton))
			return;

		if (pComponentWidget->IsEnabled())
		{
			pComponentWidget->Disable();
			pIcon->SetTextureId(m_collapsedIcon);

		}
		else
		{
			pComponentWidget->Enable();
			pIcon->SetTextureId(m_expandedIcon);
		}
		Widgets::WidgetMgr::Get().RequestResize();
	}
}
