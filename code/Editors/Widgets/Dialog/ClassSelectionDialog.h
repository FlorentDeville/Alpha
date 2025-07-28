/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"

#include "Widgets/ModalWindow.h"

namespace Widgets
{
	class TableView;
}

namespace Editors
{
	class ClassModel;

	class ClassSelectionDialog : public Widgets::ModalWindow
	{
	public:
		ClassSelectionDialog(const Core::Sid& baseClassSid);
		~ClassSelectionDialog();

		void Open();
		void Close() const;

		EVENT_DECL(Ok, void(const Core::Sid& classNameSid))

	private:
		Widgets::TableView* m_pClassView;
		ClassModel* m_pClassModel;

		void Internal_OnOk();
		void Internal_OnCancel();
	};
}

