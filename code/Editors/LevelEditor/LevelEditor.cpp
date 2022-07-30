/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/LevelEditor.h"

#include "GameInputs/Inputs/InputMgr.h"

#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport.h"

namespace Editors
{
	LevelEditor::LevelEditor()
	{}

	LevelEditor::~LevelEditor()
	{
		delete m_pRenderTarget;
	}

	void LevelEditor::CreateEditor(Widget* pParent)
	{
		//create the render target
		m_pRenderTarget = RenderModule::Get().CreateRenderTarget(1280, 720);

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();

		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pViewport->OnGetFocus([]() -> bool { GameInputs::InputMgr::Get().Enable(); return true; });
		pViewport->OnLoseFocus([]() -> bool { GameInputs::InputMgr::Get().Disable(); return true; });
		pViewport->OnGetRenderTargetTexture([this]() -> TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });
		pViewportTab->AddWidget(pViewport);

		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Level", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}
	}

	void LevelEditor::Update()
	{}

	void LevelEditor::Render()
	{

	}
}
