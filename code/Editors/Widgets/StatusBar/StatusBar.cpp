/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/StatusBar/StatusBar.h"

#include "Core/Log/LogModule.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"

namespace Editors
{
	StatusBar::StatusBar()
		: Widgets::Layout()
	{
		SetDirection(Widgets::Layout::Horizontal);
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_FIT);

		Widgets::Container* pSpacer = new Widgets::Container(10, 20);
		AddWidget(pSpacer);

		m_pLabel = new Widgets::Label();
		AddWidget(m_pLabel);

		Core::LogModule::Get().OnLogAdded([this](const Core::LogModule::LogEntry& entry) { SetText(entry.m_text); });
	}

	StatusBar::~StatusBar()
	{ }

	void StatusBar::SetText(const std::string& text)
	{
		m_pLabel->SetText(text);
	}
}
