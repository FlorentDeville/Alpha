/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/Widgets/CameraWidget.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderTargets/RenderTarget.h"

#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/Objects/GameObject.h"

#include "Widgets/WidgetMgr.h"

//needed for the operator* between vectors and floats.
using namespace DirectX;

//#pragma optimize("", off)

namespace Editors
{
	struct ConstBufferPerObject
	{
		Core::Mat44f m_world;
		float m_objectId[3];
		float m_padding;
	};

	struct ConstBufferPerFrame
	{
		Core::Mat44f m_view;
		Core::Mat44f m_proj;
	};

	LevelEditorViewportWidget::LevelEditorViewportWidget(int width, int height)
		: Widgets::Viewport_v2(width, height)
		, m_enableViewportControl(false)
		, m_isPanning(false)
	{
		m_pCamera = new CameraWidget();
		m_pCamera->OnWsChanged([](const Core::Mat44f& mat) { LevelEditorModule::Get().SetCameraWs(mat); });

		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMVECTOR cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);
		m_pCamera->SetTransform(cameraPosition, cameraEulerAngle);

		m_pGizmoModel = new GizmoModel();
		m_pGizmoWidget = new GizmoWidget();

		m_pGizmoWidget->SetModel(m_pGizmoModel);

		m_pObjectIdRenderTarget = Rendering::RenderModule::Get().CreateRenderTarget(width, height);
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{
		delete m_pCamera;
		delete m_pGizmoWidget;
		delete m_pObjectIdRenderTarget;
	}

	void LevelEditorViewportWidget::Render()
	{
		m_pCamera->Render(m_aspectRatio);

		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		Systems::LevelAsset* pLevel = levelEditorModule.GetCurrentLoadedLevel();
		if (!pLevel)
			return;

		const Core::Array<Systems::GameObject*>& roots = pLevel->GetRootGameObjects();
		for (Systems::GameObject* pRoot : roots)
			pRoot->UpdateTransform();

		const Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();
		for (Systems::GameObject* pGo : gameObjects)
			pGo->Render();

		m_pRenderTarget->ClearDepthBuffer();

		m_pGizmoWidget->Render();
	}

	void LevelEditorViewportWidget::Update(uint64_t dt)
	{
		if (!m_enableViewportControl)
			return;

		float dtInSeconds = dt / 1000.f;

		DirectX::XMUINT2 mouseAbsPos;
		Inputs::InputMgr::Get().GetMousePosition(mouseAbsPos.x, mouseAbsPos.y);
		DirectX::XMVECTOR mouse3dPosition = Compute3dPosition(mouseAbsPos);
		Core::Vec4f mouseWs(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);

		m_pCamera->Update(dtInSeconds);
		m_pGizmoWidget->Update(mouseWs);

		{
			Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
			Systems::LevelAsset* pLevel = levelEditorModule.GetCurrentLoadedLevel();
			if (!pLevel)
				return;

			const Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();
			for (Systems::GameObject* pGo : gameObjects)
				pGo->Update();
		}
	}

	void LevelEditorViewportWidget::SetEnableViewportControl(bool enable)
	{
		m_enableViewportControl = enable;
	}

	GizmoWidget* LevelEditorViewportWidget::GetGizmoWidget()
	{
		return m_pGizmoWidget;
	}

	GizmoModel* LevelEditorViewportWidget::GetGizmoModel()
	{
		return m_pGizmoModel;
	}

	void LevelEditorViewportWidget::Internal_Render()
	{
		m_pRenderTarget->BeginScene();
		Render();
		m_pRenderTarget->EndScene();


		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		Systems::LevelAsset* pLevel = levelEditorModule.GetCurrentLoadedLevel();
		if (pLevel)
		{
			Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
			Rendering::PipelineStateId objectIdsPsoId = widgetMgr.GetObjectIdsPsoId();
			Rendering::PipelineState* pPso = Rendering::PipelineStateMgr::Get().GetPipelineState(objectIdsPsoId);

			Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

			ConstBufferPerFrame perFrame;
			perFrame.m_view.m_matrix = renderModule.GetConstCamera()->GetViewMatrix();
			perFrame.m_proj.m_matrix = renderModule.GetConstCamera()->GetProjectionMatrix();

			m_pObjectIdRenderTarget->BeginScene();

			renderModule.BindMaterial(*pPso);

			renderModule.SetConstantBuffer(1, sizeof(ConstBufferPerFrame), &perFrame, 0);

			const Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();
			for (const Systems::GameObject* pGo : gameObjects)
			{
				const Core::Array<Systems::GameComponent*> components = pGo->GetComponents();
				for (const Systems::GameComponent* pComponent : components)
				{
					const Systems::StaticMeshComponent* pMeshComponent = dynamic_cast<const Systems::StaticMeshComponent*>(pComponent);
					if (!pMeshComponent)
						continue;

					const Systems::MeshAsset* pMesh = pMeshComponent->GetMesh();
					if (!pMesh)
						continue;

					const Core::Mat44f& world = pGo->GetTransform().GetWorldTx();
					ConstBufferPerObject perObject;
					perObject.m_world = world;
					perObject.m_objectId[0] = 1;
					perObject.m_objectId[1] = 0;
					perObject.m_objectId[2] = 0;

					renderModule.SetConstantBuffer(0, sizeof(ConstBufferPerObject), &perObject, 0);

					const Rendering::Mesh* pRenderingMesh = pMesh->GetRenderingMesh();
					renderModule.RenderMesh(*pRenderingMesh);
				}
			}


			m_pObjectIdRenderTarget->EndScene();
		}
	}
}