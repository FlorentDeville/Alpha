/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "GameInputs/Inputs/InputMgr.h"

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/ShaderMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Split2Way.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Viewport.h"

#include <filesystem>

extern std::string g_shaderRoot;

namespace Editors
{
	MeshEditor::MeshEditor()
		: Core::Singleton<MeshEditor>()
		, m_pRenderTarget(nullptr)
	{
		m_meshToRender.m_id = -1;
	}

	MeshEditor::~MeshEditor()
	{
		delete m_pRenderTarget;
	}

	void MeshEditor::CreateEditor(Widget* pParent)
	{
		//create the render target
		m_pRenderTarget = RenderModule::Get().CreateRenderTarget(1280, 720);

		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Mesh", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		//create the split
		Widgets::Split2Way* pSplit = new Widgets::Split2Way();
		pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pViewportTab->AddWidget(pSplit);

		//create left viewport
		Widgets::Viewport* pViewport = new Widgets::Viewport();
		pViewport->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pViewport->OnGetFocus([]() -> bool { GameInputs::InputMgr::Get().Enable(); return true; });
		pViewport->OnLoseFocus([]() -> bool { GameInputs::InputMgr::Get().Disable(); return true; });
		pViewport->OnGetRenderTargetTexture([this]() -> TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });
		pSplit->AddRightPanel(pViewport);

		//create the list of meshes
		std::string meshRoot = "c:\\workspace\\Alpha\\data\\mesh";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(meshRoot))
		{
			m_meshesFilenameList.push_back(entry.path().string());
		}

		//create a button and label per mesh
		Layout* pMeshListLayout = new Layout(0, 0, 0, 0);
		pMeshListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pMeshListLayout->SetDirection(Layout::Direction::Vertical);
		pSplit->AddLeftPanel(pMeshListLayout);

		for (const std::string& mesh : m_meshesFilenameList)
		{
			Button* pButton = new Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, mesh](int x, int y) -> bool { LoadMesh(mesh); return true; });
			pMeshListLayout->AddWidget(pButton);

			Label* pLabel = new Label(0, 0, 1, mesh);
			pButton->AddWidget(pLabel);
		}
	}

	void MeshEditor::Update()
	{}

	void MeshEditor::Render()
	{
		m_pRenderTarget->BeginScene();

		//world
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		//view
		DirectX::XMVECTOR euler = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR cameraDirection = DirectX::XMVectorSubtract(cameraLookAt, cameraPosition);
		cameraDirection = DirectX::XMVector4Normalize(cameraDirection);

		DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(euler);
		DirectX::XMVECTOR direction = DirectX::XMVector3Transform(cameraDirection, orientation);
		DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(cameraPosition, direction, cameraUp);

		//projection
		float fov = 45.f;
		const DirectX::XMUINT2 gameResolution = RenderModule::Get().GetGameResolution();
		float aspectRatio = gameResolution.x / static_cast<float>(gameResolution.y);
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(fov);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearDistance, 100.0f);

		//RENDER
		if (m_meshToRender.m_id != -1)
		{
			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);
			RenderModule::Get().Render(*g_pRenderableMgr->GetRenderable(m_meshToRender), mvpMatrix);
		}

		m_pRenderTarget->EndScene();
	}

	void MeshEditor::LoadMesh(const std::string& meshFilename)
	{
		Mesh* pCubeMesh = nullptr;
		MeshId cubeMeshId;
		g_pMeshMgr->CreateMesh(&pCubeMesh, cubeMeshId);
		pCubeMesh->Load(meshFilename);

		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\base.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.ps.cso");

		PipelineStateId pid = 0;
		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(pid, "base");
		pPipelineState->Init_PosColor(rsId, vsId, psId);

		m_meshToRender = g_pRenderableMgr->CreateRenderable(cubeMeshId, pid);
	}

}
