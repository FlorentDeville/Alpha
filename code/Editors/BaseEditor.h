/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

namespace Widgets
{
	class Layout;
	class MenuBar;
	class Widget;
}

namespace Editors
{
	class EditorParameter;
	class StatusBar;

	class BaseEditor
	{
	public:
		BaseEditor();
		virtual ~BaseEditor();

		virtual void CreateEditor(const EditorParameter& param) = 0;

	protected:
		void CreateDefaultWidgets(Widgets::Widget* pParent, const std::string& title);

	protected:
		StatusBar* m_pStatusBar;
		Widgets::MenuBar* m_pMenuBar;
		Widgets::Layout* m_pInternalLayout; //layout to use to add widgets in the editor.
	};
}
