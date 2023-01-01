/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include <DirectXMath.h>

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

	private:
		BaseModel* m_pModel;

		Widgets::Layout* m_pArrayLayout;

		std::vector<int> m_columnSize;

		bool m_isDirtyWidget;
		int m_selectedRow;

		char m_padding[8];
		DirectX::XMVECTOR m_evenRowBackgroundColor;
		DirectX::XMVECTOR m_oddRowBackgroundColor;
		DirectX::XMVECTOR m_hoverBackgroundColor;
	};
}