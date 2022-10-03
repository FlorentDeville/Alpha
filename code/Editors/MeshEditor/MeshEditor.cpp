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
	MeshEntry::MeshEntry()
		: m_filename()
	{
		m_meshId.m_id = -1;
		m_renderableId.m_id = -1;
	}

	MeshEditor::MeshEditor()
		: Core::Singleton<MeshEditor>()
		, m_pRenderTarget(nullptr)
		, m_selectedMesh(-1)
		, m_cameraDistance(10.f)
		, m_pid(-1)
	{
		m_cameraEuler = DirectX::XMVectorSet(0, 0, 0, 1);
		m_cameraTarget = DirectX::XMVectorSet(0, 0, 0, 1);
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
			m_allMeshes.push_back(MeshEntry());
			m_allMeshes.back().m_filename = entry.path().string();
		}

		//create a button and label per mesh
		Layout* pMeshListLayout = new Layout(0, 0, 0, 0);
		pMeshListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pMeshListLayout->SetDirection(Layout::Direction::Vertical);
		pSplit->AddLeftPanel(pMeshListLayout);

		for(int ii = 0; ii < m_allMeshes.size(); ++ii)
		{
			const MeshEntry& entry = m_allMeshes[ii];

			const std::string& meshName = entry.m_filename;
			Button* pButton = new Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pMeshListLayout->AddWidget(pButton);

			Label* pLabel = new Label(0, 0, 1, meshName);
			pButton->AddWidget(pLabel);
		}

		//load the material to render the mesh
		RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\base.rs.cso");
		ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.vs.cso");
		ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.ps.cso");

		PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(m_pid, "base");
		pPipelineState->Init_PosColor(rsId, vsId, psId);
	}

	void MeshEditor::Update()
	{
		DirectX::XMVECTOR ySpeed = DirectX::XMVectorSet(0, 0.05f, 0, 0);
		DirectX::XMVECTOR xSpeed = DirectX::XMVectorSet(0.05f, 0, 0, 0);

		//I should not use the game input but it's good enough for now
		GameInputs::InputMgr& inputs = GameInputs::InputMgr::Get();
		if (inputs.GetState(GameInputs::InputCommand::MoveLeft))
		{
			m_cameraEuler = DirectX::XMVectorSubtract(m_cameraEuler, ySpeed);
		}
		else if (inputs.GetState(GameInputs::InputCommand::MoveRight))
		{
			m_cameraEuler = DirectX::XMVectorAdd(m_cameraEuler, ySpeed);
		}

		if (inputs.GetState(GameInputs::InputCommand::MoveForward))
		{
			m_cameraEuler = DirectX::XMVectorSubtract(m_cameraEuler, xSpeed);
		}
		else if (inputs.GetState(GameInputs::InputCommand::MoveBackward))
		{
			m_cameraEuler = DirectX::XMVectorAdd(m_cameraEuler, xSpeed);
		}
	}

	void MeshEditor::Render()
	{
		m_pRenderTarget->BeginScene();

		//world
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		//view
		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVectorSet(0, 0, 0, 1);

		//calculate the camera position
		DirectX::XMMATRIX orientation = DirectX::XMMatrixRotationRollPitchYawFromVector(m_cameraEuler);
		DirectX::XMMATRIX tx = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(0, 0, m_cameraDistance, 1));
		DirectX::XMMATRIX txPos = DirectX::XMMatrixMultiply(tx, orientation);
		DirectX::XMVECTOR cameraPosition = DirectX::XMVector3Transform(m_cameraTarget, txPos);
		

		DirectX::XMVECTOR cameraDirection = DirectX::XMVectorSubtract(cameraLookAt, cameraPosition);
		cameraDirection = DirectX::XMVector4Normalize(cameraDirection);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(cameraPosition, cameraDirection, cameraUp);

		//projection
		float fov = 45.f;
		const DirectX::XMUINT2 gameResolution = RenderModule::Get().GetGameResolution();
		float aspectRatio = gameResolution.x / static_cast<float>(gameResolution.y); //ratio shoudl be based on the viewport size, not the game resolution
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(fov);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearDistance, 100.0f);

		//RENDER
		if (m_selectedMesh != -1)
		{
			const MeshEntry& entry = m_allMeshes[m_selectedMesh];

			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);
			RenderModule::Get().Render(*g_pRenderableMgr->GetRenderable(entry.m_renderableId), mvpMatrix);
		}

		m_pRenderTarget->EndScene();
	}

	void MeshEditor::ShowMesh(int entryIndex)
	{
		m_selectedMesh = entryIndex;
	}

	void MeshEditor::LoadMesh(const std::string& meshFilename, MeshEntry& entry)
	{
		entry.m_filename = meshFilename;

		Mesh* pCubeMesh = nullptr;
		g_pMeshMgr->CreateMesh(&pCubeMesh, entry.m_meshId);
		pCubeMesh->Load(meshFilename);

		entry.m_renderableId = g_pRenderableMgr->CreateRenderable(entry.m_meshId, m_pid);
	}

	void MeshEditor::OnMeshEntryClicked(int entryIndex)
	{
		MeshEntry& entry = m_allMeshes[entryIndex];

		if (entry.m_renderableId.m_id == -1)
		{
			LoadMesh(entry.m_filename, entry);
		}

		ShowMesh(entryIndex);
	}
}
