/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MeshEditor/MeshEditor.h"

#include "GameInputs/Inputs/InputMgr.h"

#include "Rendering/Material/Material.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Mesh/Mesh.h"
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
	}

	MeshEditor::MeshEditor()
		: Core::Singleton<MeshEditor>()
		, m_pRenderTarget(nullptr)
		, m_selectedMesh(-1)
		, m_cameraDistance(10.f)
		, m_materialId(Rendering::MaterialId::INVALID)
		, m_aspectRatio(0.f)
		, m_enableViewportControl(false)
		, m_firstFrameMouseDown(true)
		, m_mousePreviousPos(0, 0)
		, m_pMeshListLayout(nullptr)
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
		const int width = 1280;
		const int height = 720;
		m_aspectRatio = width / static_cast<float>(height);
		m_pRenderTarget = RenderModule::Get().CreateRenderTarget(width, height);

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
		pViewport->OnGetRenderTargetTexture([this]() -> TextureId { return RenderModule::Get().GetRenderTargetTextureId(m_pRenderTarget); });
		pViewport->OnGetFocus([this]() -> bool { m_enableViewportControl = true; return true; });
		pViewport->OnLoseFocus([this]() -> bool { m_enableViewportControl = false; return true; });
		pSplit->AddRightPanel(pViewport);

		//create the list of meshes
		std::string meshRoot = "c:\\workspace\\Alpha\\data\\mesh";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(meshRoot))
		{
			m_allMeshes.push_back(MeshEntry());
			m_allMeshes.back().m_filename = entry.path().string();
		}

		//create a button and label per mesh
		m_pMeshListLayout = new Widgets::Layout(0, 0, 0, 0);
		m_pMeshListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pMeshListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddLeftPanel(m_pMeshListLayout);

		for(int ii = 0; ii < m_allMeshes.size(); ++ii)
		{
			const MeshEntry& entry = m_allMeshes[ii];

			const std::string& meshName = entry.m_filename;
			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			m_pMeshListLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, meshName);
			pButton->AddWidget(pLabel);
		}

		//load the material to render the mesh
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();
		Rendering::Material* pMaterial = nullptr;
		materialMgr.CreateMaterial(&pMaterial, m_materialId);

		{
			RootSignatureId rsId = g_pRootSignatureMgr->CreateRootSignature(g_shaderRoot + "\\base.rs.cso");
			ShaderId vsId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.vs.cso");
			ShaderId psId = g_pShaderMgr->CreateShader(g_shaderRoot + "\\base.ps.cso");
			
			PipelineStateId pid;
			PipelineState* pPipelineState = g_pPipelineStateMgr->CreateResource(pid, "base");
			pPipelineState->Init_PosColor(rsId, vsId, psId);

			pMaterial->Init(rsId, pid);
		}
	}

	void MeshEditor::Update()
	{
		if (m_enableViewportControl)
		{
			GameInputs::InputMgr& inputs = GameInputs::InputMgr::Get();
			if (inputs.IsMouseLeftButtonDown())
			{
				DirectX::XMUINT2 mousePosition;
				inputs.GetMousePosition(mousePosition.x, mousePosition.y);
				if (m_firstFrameMouseDown)
				{
					m_mousePreviousPos = mousePosition;
					m_firstFrameMouseDown = false;
				}

				DirectX::XMINT2 delta;
				delta.x = m_mousePreviousPos.x - mousePosition.x;
				delta.y = m_mousePreviousPos.y - mousePosition.y;

				const float ROTATION_SPEED = 0.01f;
				DirectX::XMVECTOR offset = DirectX::XMVectorSet(static_cast<float>(delta.y) * ROTATION_SPEED, -static_cast<float>(delta.x) * ROTATION_SPEED, 0, 0);

				m_cameraEuler = DirectX::XMVectorAdd(m_cameraEuler, offset);

				m_mousePreviousPos = mousePosition;
			}
			else if (!m_firstFrameMouseDown)
			{
				m_firstFrameMouseDown = true;
			}
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
		const float fov = 45.f;
		float nearDistance = 0.1f;
		float fovRad = DirectX::XMConvertToRadians(fov);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, m_aspectRatio, nearDistance, 100.0f);

		//RENDER
		if (m_selectedMesh != -1)
		{
			const MeshEntry& entry = m_allMeshes[m_selectedMesh];

			DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(world, view);
			mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

			RenderModule& renderer = RenderModule::Get();

			const Rendering::Material* pMaterial = Rendering::MaterialMgr::Get().GetMaterial(m_materialId);
			renderer.BindMaterial(*pMaterial, mvpMatrix);

			const Mesh* pMesh = g_pMeshMgr->GetMesh(entry.m_meshId);
			renderer.RenderMesh(*pMesh);
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
	}

	void MeshEditor::OnMeshEntryClicked(int entryIndex)
	{
		//deselect all buttons
		const std::vector<Widget*>& allButtons = m_pMeshListLayout->GetChildren();
		for (Widget* pWidget : allButtons)
		{
			Widgets::Button* pButton = static_cast<Widgets::Button*>(pWidget);
			pButton->Unselect();
		}

		//select new button
		Widget* pWidget = allButtons[entryIndex];
		Widgets::Button* pButton = static_cast<Widgets::Button*>(pWidget);
		pButton->Select();

		MeshEntry& entry = m_allMeshes[entryIndex];
		
		//load the mesh if necessary
		if (entry.m_meshId.m_id == -1)
		{
			LoadMesh(entry.m_filename, entry);
		}

		ShowMesh(entryIndex);
	}
}
