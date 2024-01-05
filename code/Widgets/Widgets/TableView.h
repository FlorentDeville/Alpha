/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Widgets/Tools/Color.h"
#include "Widgets/Widget.h"

#include <map>

namespace Widgets
{
	class AbstractViewModel;
	class Layout;
	class ModelIndex;

	class TableView : public Widget
	{
	public:
		TableView();
		~TableView();

		void SetModel(AbstractViewModel* pModel);

		void SetMultiSelection(bool enable);
		
		EVENT_DECL(ItemDoubleClick, void(const ModelIndex& idx))

	private:
		AbstractViewModel* m_pModel;

		Color m_oddRowBackgroundColor;
		Color m_evenRowBackgroundColor;
		Color m_hoverBackgroundColor;
		Color m_selectedBorderColor;

		Layout* m_pLayout;

		bool m_multiSelectionEnabled;

		void CreateView();

		void OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, int itemIndex);
		void OnMouseDoubleClick_ItemLayout(const Widgets::MouseEvent& ev, int row);

		void SetSelectedRowStyle(int row);
		void SetDeselectedRowStyle(int row);
	};
}
