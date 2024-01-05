/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

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

		Layout* m_pLayout;

		void CreateView();
	};
}
