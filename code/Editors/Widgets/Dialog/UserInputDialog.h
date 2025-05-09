/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/ModalWindow.h"

#include <string>

namespace Widgets
{
	class TextBox;
}

namespace Editors
{
	class UserInputDialog : public Widgets::ModalWindow
	{
	public:
		UserInputDialog(const std::string& title);
		~UserInputDialog();

		void Open();
		void Close() const;

		EVENT_DECL(InputValidated, void(const std::string& userInput))

	private:
		void OnOk();
		void OnCancel();

		Widgets::TextBox* m_pTextBox;
	};
}
