/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/GamePlayer/GamePlayer.h"

#include "Inputs/InputMgr.h"

#include "Rendering/RenderModule.h"

#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport_v2.h"

namespace Editors
{
	GamePlayer::GamePlayer()
	{}

	GamePlayer::~GamePlayer()
	{}

	void GamePlayer::CreateEditor(Widgets::Widget* pParent)
	{
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		Widgets::Viewport_v2* pViewport = new Widgets::Viewport_v2(WIDTH, HEIGHT);
		pViewport->SetSizeStyle(Widgets::Widget::STRETCH);
		pViewportTab->AddWidget(pViewport);

		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Game", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}
	}
}