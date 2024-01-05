/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tools/Color.h"
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

		int GetSelectedItem() const;

		EVENT_DECL(ItemDoubleClick, void(int itemIndex))

	private:
		void OnMouseDown_ItemLayout(const Widgets::MouseEvent& ev, int itemIndex);

		BaseModel* m_pModel;

		Widgets::Layout* m_pArrayLayout;

		std::vector<int> m_columnSize;

		Widgets::Color m_evenRowBackgroundColor;
		Widgets::Color m_oddRowBackgroundColor;
		Widgets::Color m_hoverBackgroundColor;

		int m_selectedRow;
		bool m_isDirtyWidget;
	};
}