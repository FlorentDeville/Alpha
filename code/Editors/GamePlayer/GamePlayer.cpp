/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/GamePlayer/GamePlayer.h"

#include "Editors/EditorParameter.h"

#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"

#include "Systems/Game/GameMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"
#include "Widgets/Viewport.h"

namespace Editors
{
	GamePlayer::GamePlayer()
		: BaseEditor()
		, m_pPso(nullptr)
		, m_pRootSig(nullptr)
	{}

	GamePlayer::~GamePlayer()
	{
		delete m_pPso;
	}

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
		pViewport->OnRender([this]() { Viewport_OnRender(); });

		m_pInternalLayout->AddWidget(pViewport);

		m_pRootSig = Rendering::RootSignatureMgr::Get().GetRootSignature(Rendering::EngineRootSigs::COPY_RENDER_TARGET);

		Rendering::PipelineStateDesc desc;
		desc.m_depthFunction = Rendering::DepthComparisonMode::Always;
		desc.m_cullMode = Rendering::CullMode::None;
		desc.m_pVs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::COPY_RENDER_TARGET_VS);
		desc.m_pPs = Rendering::ShaderMgr::Get().GetShader(Rendering::EngineShaders::COPY_RENDER_TARGET_PS);
		desc.m_pRs = m_pRootSig;

		m_pPso = new Rendering::PipelineState();
		m_pPso->Init_Generic(desc);
	}

	void GamePlayer::Viewport_OnRender()
	{
		Rendering::RenderTarget* pGameRT = Systems::GameMgr::Get().GetFinalRenderTarget();
		Rendering::Texture* pGameTexture = pGameRT->GetColorTexture();
		pGameTexture->TransitionToShaderResource();

		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();
		renderer.BindMaterial(*m_pPso, *m_pRootSig);
		
		{
			ID3D12DescriptorHeap* pSrv = pGameTexture->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };

			//I should have only 2 giant descriptor heaps, one for cbv,srv,uav and another one for sampler.
			renderer.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			renderer.GetRenderCommandList()->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());
		}

		renderer.RenderNoBufferTriangle();
	}
}