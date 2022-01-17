/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

class Widget;

namespace Editors
{
	class GamePlayer : public Core::Singleton<GamePlayer>
	{
	public:
		GamePlayer();
		~GamePlayer();

		void CreateEditor(Widget* pParent);
	};
}
