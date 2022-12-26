/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Layout;
	class Label;
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

		void Update() override;

	private:
		BaseModel* m_pModel;

		Widgets::Layout* m_pLayout;
		Widgets::Label* m_pLabel[4][3];
	};
}
