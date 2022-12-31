/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Editors
{
	class BaseModel;

	class EntityWidget : public Widgets::Widget
	{
	public:
		EntityWidget();
		~EntityWidget();

		void Update() override;

		void SetModel(BaseModel* pModel);

	private:
		BaseModel* m_pModel;

		bool m_isDirtyWidget;

		void CreateWidgets();
	};
}
