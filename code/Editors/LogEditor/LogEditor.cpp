/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LogEditor/LogEditor.h"

#include "Core/Log/LogModule.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"

namespace Editors
{
	LogEditor::LogEditor()
		: Core::Singleton<LogEditor>()
		, m_pInternalLayout(nullptr)
	{ }

	LogEditor::~LogEditor()
	{ }

	void LogEditor::CreateEditor(const LogEditorParameter& parameter)
	{
		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(parameter.m_pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Log", pViewportTab);
		}
		else
		{
			parameter.m_pParent->AddWidget(pViewportTab);
		}

		m_pInternalLayout = new Widgets::Layout();
		m_pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		m_pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pViewportTab->AddWidget(m_pInternalLayout);

		Core::LogModule& logModule = Core::LogModule::Get();
		const Core::Array<Core::LogModule::LogEntry>& allLogs = logModule.GetAllLogEntries();
		for (const Core::LogModule::LogEntry& entry : allLogs)
			AddLogLine(entry.m_text);

		logModule.OnLogAdded([this](const Core::LogModule::LogEntry& entry) { AddLogLine(entry.m_text); });
	}

	void LogEditor::AddLogLine(const std::string& text)
	{
		//strip \r\n
		
		Widgets::Label* pNewLabel = new Widgets::Label(text);
		m_pInternalLayout->AddWidget(pNewLabel);
	}
}
