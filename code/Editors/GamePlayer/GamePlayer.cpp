/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/GamePlayer/GamePlayer.h"

#include "Editors/EditorParameter.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"
#include "Widgets/Viewport.h"

namespace Editors
{
	GamePlayer::GamePlayer()
	{}

	GamePlayer::~GamePlayer()
	{}

	void GamePlayer::CreateEditor(const EditorParameter& param)
	{
		CreateDefaultWidgets(param.m_pParent, "Game");
	
		Widgets::Layout* pMenu = new Widgets::Layout(Widgets::Layout::Horizontal, Widgets::HSTRETCH_VFIT);
		m_pInternalLayout->AddWidget(pMenu);

		Widgets::Button* pPlayButton = new Widgets::Button("Play");
		pMenu->AddWidget(pPlayButton);

		Widgets::Button* pPauseButton = new Widgets::Button("Pause");
		pMenu->AddWidget(pPauseButton);

		Widgets::Button* pStopButton = new Widgets::Button("Stop");
		pMenu->AddWidget(pStopButton);

		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		Widgets::Viewport* pViewport = new Widgets::Viewport(WIDTH, HEIGHT);
		pViewport->SetSizeStyle(Widgets::STRETCH);
		m_pInternalLayout->AddWidget(pViewport);
	}
}