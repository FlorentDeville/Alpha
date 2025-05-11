/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/ModalWindow.h"

namespace Editors
{
	class OkCancelDialog : public Widgets::ModalWindow
	{
	public:
		OkCancelDialog(const std::string& title, const std::string& text);
		~OkCancelDialog();

		bool Handle(const Widgets::BaseEvent& ev) override;

		EVENT_DECL(Ok, void())
		EVENT_DECL(Cancel, void())

	private:
		void Internal_OnOk();
		void Internal_OnCancel();
	};
}
