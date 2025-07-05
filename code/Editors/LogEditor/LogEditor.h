/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <string>

namespace Widgets
{
	class Layout;
	class Widget;
}

namespace Editors
{
	class LogEditorParameter
	{
	public:
		Widgets::Widget* m_pParent;
	};

	class LogEditor : public Core::Singleton<LogEditor>
	{
	public:
		LogEditor();
		~LogEditor();

		void CreateEditor(const LogEditorParameter& parameter);

	private:
		Widgets::Layout* m_pInternalLayout;

		void AddLogLine(const std::string& text);
	};
}
