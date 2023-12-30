/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Editors
{
	class ComponentModel;

	class ComponentWidget : public Widgets::Widget
	{
	public:
		ComponentWidget();
		~ComponentWidget();

		void Update(uint64_t dt) override;

		void SetModel(ComponentModel* pModel);

	private:
		ComponentModel* m_pModel;

		bool m_isDirtyWidget;

		void CreateWidgets();
	};
}
