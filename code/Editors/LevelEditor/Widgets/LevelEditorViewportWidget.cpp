/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"

#include "Core/Math/Constants.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/RenderPassObjectId.h"
#include "Editors/LevelEditor/Widgets/CameraWidget.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Inputs/InputMgr.h"

#include "OsWin/Input.h"

#include "Rendering/Camera.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
#include "Systems/Rendering/RenderPass/RenderPassBase.h"
#include "Systems/Rendering/RenderPass/RenderPassShadowMaps.h"

#include "Widgets/Events/GlobalEvent.h"
#include "Widgets/WidgetMgr.h"

//needed for the operator* between vectors and floats.
using namespace DirectX;

//#pragma optimize("", off)

namespace Editors
{
	
	struct ConstBufferPerObject
	{
		Core::Mat44f m_world;
		uint32_t m_objectId[4];
	};

	struct ConstBufferPerFrame
	{
		Core::Mat44f m_view;
		Core::Mat44f m_proj;
	};

	LevelEditorViewportWidget::LevelEditorViewportWidget(int width, int height)
		: Widgets::Viewport(width, height)
		, m_enableViewportControl(false)
		, m_isPanning(false)
	{
		m_pCamera = new CameraWidget();
		m_pCamera->OnWsChanged([](const Core::Mat44f& mat) { LevelEditorModule::Get().SetCameraWs(mat); });
		m_pCamera->SetFov(LevelEditorModule::Get().GetFovRad());

		Core::Vec4f cameraPosition(0, 10, -10, 1);
		Core::Vec4f cameraEulerAngle(Core::PI * 0.25f, 0, 0, 0);
		m_pCamera->SetTransform(cameraPosition, cameraEulerAngle);

		m_pGizmoModel = new GizmoModel();
		m_pGizmoWidget = new GizmoWidget();

		m_pGizmoWidget->SetModel(m_pGizmoModel);

		Rendering::TextureId readbackTextureId;
		Rendering::TextureMgr::Get().CreateTexture(&m_pReadbackBuffer, readbackTextureId);
		m_pReadbackBuffer->InitAsReadbackBuffer(width, height, 12);

		m_pRenderPassShadowMaps = new Systems::RenderPassShadowMaps();
		m_pRenderPassObjectId = new RenderPassObjectId(width, height);

		m_pRenderPassBase = new Systems::RenderPassBase();
		m_pRenderPassBase->SetRenderTarget(m_pRenderTarget);
		m_pRenderPassBase->SetShadowMapRenderTargets(m_pRenderPassShadowMaps->GetRenderTargets(), m_pRenderPassShadowMaps->GetRenderTargetsCount(), m_pRenderPassShadowMaps->GetSrvHeap());
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{
		delete m_pCamera;
		delete m_pGizmoWidget;
		delete m_pRenderPassShadowMaps;
		delete m_pRenderPassObjectId;
		delete m_pRenderPassBase;
	}

	void LevelEditorViewportWidget::Update(uint64_t dt)
	{
		if (!m_enableViewportControl)
			return;

		float dtInSeconds = dt / 1000.f;

		Core::Int2 mouseAbsPos = Inputs::InputMgr::Get().GetMousePosition();

		//clamp mouse abs position to the viewport
		Core::UInt2 clampedMousePos;
		if (mouseAbsPos.x < GetScreenX())
			clampedMousePos.x = GetScreenX();
		else if (mouseAbsPos.x > static_cast<int32_t>(GetScreenX() + GetWidth()))
			clampedMousePos.x = GetScreenX() + GetWidth();
		else
			clampedMousePos.x = mouseAbsPos.x;

		if (mouseAbsPos.y < GetScreenY())
			clampedMousePos.y = GetScreenY();
		else if (mouseAbsPos.y > static_cast<int32_t>(GetScreenY() + GetHeight()))
			clampedMousePos.y = GetScreenY() + GetHeight();
		else
			clampedMousePos.y = mouseAbsPos.y;

		Core::Vec4f mouseWs = Compute3dPosition(clampedMousePos, m_pCamera->GetView(), m_pCamera->GetProjection());

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

	bool LevelEditorViewportWidget::Handle(const Widgets::GlobalEvent& event)
	{
		bool handled = false;

		if(event.m_id == Widgets::EventType::kMouseDown && !m_pGizmoWidget->IsManipulating() && !m_pGizmoWidget->IsHovering())
		{
			const Widgets::MouseEvent& mouseEvent = event.m_param.m_mouseEvent;
			if (mouseEvent.HasButton(Widgets::MouseButton::LeftButton))
			{
				handled = true;
				int widgetPixelX = mouseEvent.GetX() - GetScreenX();
				int widgetPixelY = mouseEvent.GetY() - GetScreenY();

				uint32_t objectId = GetObjectId(widgetPixelX, widgetPixelY);
				if (objectId == 0)
					return handled;

				const Core::Guid* pSelectedGuid = m_pRenderPassObjectId->GetGuid(objectId);
				if (!pSelectedGuid)
					return handled;
				
				bool toggle = false;
				if (Os::IsKeyDown(Os::VKeyCodes::Control))
					toggle = true;

				LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
				bool isAlreadySelected = levelEditorModule.IsSelected(*pSelectedGuid);
				if (isAlreadySelected && toggle)
				{
					levelEditorModule.RemoveFromSelection(*pSelectedGuid);
				}
				else if (!isAlreadySelected)
				{
					levelEditorModule.ClearSelection();
					levelEditorModule.AddToSelection(*pSelectedGuid);
				}

				return handled;
			}
		}

		return Viewport::Handle(event);
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
		//compute the camera
		m_pCamera->Render(m_aspectRatio);

		//create the render scene
		Systems::RenderableScene scene;
		{
			Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
			Systems::LevelAsset* pLevel = levelEditorModule.GetCurrentLoadedLevel();
			if (!pLevel)
			{
				//render an empty scene
				m_pRenderTarget->BeginScene();
				m_pRenderTarget->EndScene();
				return;
			}

			const Core::Array<Systems::GameObject*>& roots = pLevel->GetRootGameObjects();
			for (Systems::GameObject* pRoot : roots)
				pRoot->UpdateTransform();

			Systems::PrepareRenderableCamera(m_pCamera->GetView(), m_pCamera->GetProjection(), m_pCamera->GetPosition(), m_pCamera->GetFov(), scene);
			Systems::PrepareRenderableScene(pLevel, scene);
		}

		//first do the shadow maps
		m_pRenderPassShadowMaps->PreRender(scene);
		m_pRenderPassShadowMaps->Render(scene);
		m_pRenderPassShadowMaps->PostRender(scene);

		//now render the level editor view of the scene.
		m_pRenderPassBase->PreRender(scene);
		m_pRenderPassBase->Render(scene);
		
		m_pRenderPassBase->ClearDepthBuffer();

		Core::Mat44f viewProj = m_pCamera->GetView() * m_pCamera->GetProjection();
		m_pGizmoWidget->Render(viewProj);

		m_pRenderPassBase->PostRender(scene);

		//now render the object ids view
		m_pRenderPassObjectId->PreRender(scene);
		m_pRenderPassObjectId->Render(scene);
		m_pRenderPassObjectId->PostRender(scene);

		m_pRenderPassObjectId->GetRenderTarget()->CopyToReabackBuffer(m_pReadbackBuffer);
	}

	uint32_t LevelEditorViewportWidget::GetObjectId(int mouseX, int mouseY) const
	{
		float uvX = mouseX / static_cast<float>(GetWidth());
		float uvY = mouseY / static_cast<float>(GetHeight());

		float textureRatio = m_width / static_cast<float>(m_height);

		float stretchTextureHeight = static_cast<float>(GetHeight());
		float stretchTextureWidth = stretchTextureHeight * textureRatio;
		if (stretchTextureWidth < GetWidth())
		{
			stretchTextureWidth = static_cast<float>(GetWidth());
			stretchTextureHeight = stretchTextureWidth / textureRatio;
		}

		float dw = stretchTextureWidth - GetWidth();
		float pixelOffsetX = dw / 2;
		float uvOffsetX = pixelOffsetX / stretchTextureWidth;

		float dh = stretchTextureHeight - GetHeight();
		float pixelOffsetY = dh / 2;
		float uvOffsetY = pixelOffsetY / stretchTextureHeight;

		float uvXStart = uvOffsetX;
		float uvXEnd = 1 - uvOffsetX;
		float uvYStart = uvOffsetY;
		float uvYEnd = 1 - uvOffsetY;

		int pixelX = static_cast<int>((uvX * (uvXEnd - uvXStart) + uvXStart) * m_width);
		int pixelY = static_cast<int>((uvY * (uvYEnd - uvYStart) + uvYStart) * m_height);

		int backBufferIndex = pixelY * m_width + pixelX;

		FLOAT* pReadbackBufferData = nullptr;
		HRESULT res = m_pReadbackBuffer->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&pReadbackBufferData));

		//read data here
		uint32_t objectId = *reinterpret_cast<uint32_t*>(&pReadbackBufferData[backBufferIndex]);

		//char buffer[500];
		//sprintf_s(buffer, 500, "[%d %d] Object id %d \n", pixelX, pixelY, objectId);
		//OutputDebugString(buffer);

		D3D12_RANGE emptyRange{ 0, 0 };
		m_pReadbackBuffer->GetResource()->Unmap(0, &emptyRange);

		return objectId;
	}
}