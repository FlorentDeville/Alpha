/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/BaseEditor.h"

#include "Editors/Widgets/StatusBar/StatusBar.h"

#include "Widgets/Layout.h"
#include "Widgets/MenuBar.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"

namespace Editors
{
	BaseEditor::BaseEditor()
		: m_pStatusBar(nullptr)
	{ }

	BaseEditor::~BaseEditor()
	{ }

	void BaseEditor::CreateDefaultWidgets(Widgets::Widget* pParent, const std::string& title)
	{
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab(title, pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		Widgets::Layout* pOutsideLayout = new Widgets::Layout();
		pOutsideLayout->SetDirection(Widgets::Layout::Vertical_Reverse);
		pOutsideLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		pViewportTab->AddWidget(pOutsideLayout);

		m_pStatusBar = new StatusBar();
		pOutsideLayout->AddWidget(m_pStatusBar);

		m_pInternalLayout = new Widgets::Layout();
		m_pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		m_pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pOutsideLayout->AddWidget(m_pInternalLayout);

		m_pMenuBar = new Widgets::MenuBar();
		m_pInternalLayout->AddWidget(m_pMenuBar);
	}
}
