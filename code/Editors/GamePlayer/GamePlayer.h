/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

namespace Rendering
{
	class PipelineState;
	class RootSignature;
}

namespace Widgets
{
	class Button;
	class Viewport;
}

namespace Editors
{
	class GamePlayer : public BaseEditor
	{
	public:
		GamePlayer();
		~GamePlayer();

		void CreateEditor(const EditorParameter& param) override;

	private:
		Widgets::Viewport* m_pViewport;

		Rendering::PipelineState* m_pPso;
		Rendering::RootSignature* m_pRootSig;

		Widgets::Button* m_pShowCollisionButton;

		void Viewport_OnRender();

		void OnClick_Play();
		void OnClick_Pause();
		void OnClick_Stop();
		void OnClick_ShowCollison();
	};
}
