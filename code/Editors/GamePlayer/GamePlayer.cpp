/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/GamePlayer/GamePlayer.h"

#include "Editors/EditorParameter.h"

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
	
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		Widgets::Viewport* pViewport = new Widgets::Viewport(WIDTH, HEIGHT);
		pViewport->SetSizeStyle(Widgets::STRETCH);
		m_pInternalLayout->AddWidget(pViewport);
	}
}