/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class GamePlayer : public BaseEditor
	{
	public:
		GamePlayer();
		~GamePlayer();

		void CreateEditor(const EditorParameter& param) override;
	};
}
