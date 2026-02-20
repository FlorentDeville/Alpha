/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

//#include "Core/Singleton.h"

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class BaseEditor
	{
	public:
		BaseEditor();
		virtual ~BaseEditor();

		virtual void CreateEditor(Widgets::Widget* pParent);
	};
}
