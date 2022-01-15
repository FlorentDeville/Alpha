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

	bool Tab::Handle(const Message& /*msg*/)
	{
		//If the message reached the tab then handled it no matter what so lower tab won't receive it.
		return true;
	}
}
