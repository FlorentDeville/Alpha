/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Button.h"

#include <string>

namespace Widgets
{
	class Icon;
	class Label;
	class Shortcut;

	class MenuItem : public Button
	{
	public:
		using ParentClass = Button;

		MenuItem(const std::string& name);
		~MenuItem();

		bool Handle(const BaseEvent& ev) override;
		void Enable(bool recursive = true) override;

		void SetShortcut(const std::string& shortcut);
		void SetChecked(bool check);
		void SetText(const std::string& text);

		bool IsChecked() const;

	private:
		Label* m_pLabel;
		Label* m_pShortcutLabel;
		Icon* m_pCheckmarkIcon;

		Shortcut* m_pShortcut;

		bool m_isChecked;
	};
}
