/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/ModalWindow.h"

namespace Widgets
{
	class AbstractViewModel;
	class ModelIndex;
	class TableView;
}

namespace Editors
{
	class TableDialog : public Widgets::ModalWindow
	{
	public:
		TableDialog(const std::string& title, Widgets::AbstractViewModel* pModel);
		~TableDialog();

		void Open();
		void Close() const;

		EVENT_DECL(Ok, void(Widgets::ModelIndex))

	private:
		Widgets::TableView* m_pTable;

		void OnOk();
		void OnCancel();
	};
}
