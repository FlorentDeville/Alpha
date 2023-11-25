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

	void GamePlayer::CreateEditor(Widgets::Widget* pParent)
	{
		Widgets::Tab* pViewportTab = new Widgets::Tab();

		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pViewport->OnFocusGained([](const Widgets::FocusEvent&) { Inputs::InputMgr::Get().Enable(); });
		pViewport->OnFocusLost([](const Widgets::FocusEvent&) { Inputs::InputMgr::Get().Disable(); });
		pViewport->OnGetRenderTargetTexture([]() -> Rendering::TextureId { return Rendering::RenderModule::Get().GetGameRenderTargetTextureId(); });
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