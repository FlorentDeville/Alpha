/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Editors
{
	class BaseModel;

	class TreeWidget : public Widgets::Widget
	{
	public:
		TreeWidget();
		~TreeWidget();

		void SetModel(const BaseModel* pModel);

		void Update() override;

	private:
		const BaseModel* m_pModel;

		bool m_isDirtyWidget;

		void CreateWidgets();
	};
}
