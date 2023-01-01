/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Layout;
}

namespace Editors
{
	class BaseModel;

	class ListWidget : public Widgets::Widget
	{
	public:
		ListWidget();
		~ListWidget();

		void Update(uint64_t dt) override;

		void SetModel(BaseModel* pModel);

		void SetColumnSize(int columnIndex, int size);

	private:
		BaseModel* m_pModel;

		Widgets::Layout* m_pArrayLayout;

		std::vector<int> m_columnSize;

		bool m_isDirtyWidget;
	};
}