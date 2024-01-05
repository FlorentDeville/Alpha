/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tools/Color.h"
#include "Widgets/Widget.h"

namespace Widgets
{
	class AbstractViewModel;
	class Layout;

	class TableView : public Widget
	{
	public:
		TableView();
		~TableView();

		void SetModel(AbstractViewModel* pModel);

		void SetColumnCount(int columnCount);

	private:
		AbstractViewModel* m_pModel;
		int m_columnCount;
		
		Color m_oddRowBackgroundColor;
		Color m_evenRowBackgroundColor;
		Color m_hoverBackgroundColor;

		Layout* m_pLayout;

		void CreateView();
	};
}
