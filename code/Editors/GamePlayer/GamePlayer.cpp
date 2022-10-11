/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/GamePlayer/GamePlayer.h"

#include "Inputs/InputMgr.h"

#include "Rendering/RenderModule.h"

#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport.h"

namespace Editors
{
	GamePlayer::GamePlayer()
	{}

	GamePlayer::~GamePlayer()
	{}

	void GamePlayer::CreateEditor(Widget* pParent)
	{
		Widgets::Tab* pViewportTab = new Widgets::Tab();

		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pViewport->OnGetFocus([]() -> bool { Inputs::InputMgr::Get().Enable(); return true; });
		pViewport->OnLoseFocus([]() -> bool { Inputs::InputMgr::Get().Disable(); return true; });
		pViewport->OnGetRenderTargetTexture([]() -> TextureId { return RenderModule::Get().GetGameRenderTargetTextureId(); });
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