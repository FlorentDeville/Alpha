/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Layout;
	class TextBox;
}

namespace Editors
{
	class BaseModel;

	class MatrixWidget : public Widgets::Widget
	{
	public:
		MatrixWidget();
		~MatrixWidget();

		void SetModel(BaseModel* pModel);

		void Update(uint64_t dt) override;

	private:
		BaseModel* m_pModel;

		Widgets::Layout* m_pLayout;
		Widgets::TextBox* m_pTextBox[4][3];

		bool m_isDirtyValue;
	};
}
