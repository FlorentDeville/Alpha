/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Editors
{
	class BaseModel;

	class ComponentWidget : public Widgets::Widget
	{
	public:
		ComponentWidget();
		~ComponentWidget();

		void Update() override;

		void SetModel(const BaseModel* pModel);

	private:
		const BaseModel* m_pModel;

		bool m_isDirtyWidget;

		void CreateWidgets();
	};
}
