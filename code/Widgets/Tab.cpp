/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Tab.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Tab::Tab()
		: Widget()
	{
		SetSizeStyle(HSIZE_STRETCH | VSIZE_STRETCH);
	}

	Tab::~Tab()
	{}

	bool Tab::Handle(const BaseEvent& /*ev*/)
	{
		return false;
	}
}
