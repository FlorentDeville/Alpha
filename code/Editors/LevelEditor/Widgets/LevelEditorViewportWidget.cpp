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
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

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
		uint32_t m_objectId[4];
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
		, m_objectIdToGuid()
	{
		m_pCamera = new CameraWidget();
		m_pCamera->OnWsChanged([](const Core::Mat44f& mat) { LevelEditorModule::Get().SetCameraWs(mat); });

		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMVECTOR cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);
		m_pCamera->SetTransform(cameraPosition, cameraEulerAngle);

		m_pGizmoModel = new GizmoModel();
		m_pGizmoWidget = new GizmoWidget();

		m_pGizmoWidget->SetModel(m_pGizmoModel);

		m_pObjectIdRenderTarget = new Rendering::RenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UINT, Core::Vec4f(0, 0, 0, 0));

		Rendering::TextureId readbackTextureId;
		Rendering::TextureMgr::Get().CreateTexture(&m_pReadbackBuffer, readbackTextureId);
		m_pReadbackBuffer->InitAsReadbackBuffer(width, height, 12);
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

	bool LevelEditorViewportWidget::Handle(const Widgets::BaseEvent& event)
	{
		bool handled = false;

		if(event.m_id == Widgets::EventType::kMouseUp && !m_pGizmoWidget->IsManipulating())
		{
			const Widgets::MouseEvent& mouseEvent = static_cast<const Widgets::MouseEvent&>(event);
			if (mouseEvent.HasButton(Widgets::MouseButton::LeftButton))
			{
				handled = true;
				int widgetPixelX = mouseEvent.GetX() - GetScreenX();
				int widgetPixelY = mouseEvent.GetY() - GetScreenY();

				uint32_t objectId = GetObjectId(widgetPixelX, widgetPixelY);
				if (objectId == 0)
					return handled;

				LevelEditorModule::Get().ClearSelection();

				std::map<uint32_t, Core::Guid>::const_iterator it = m_objectIdToGuid.find(objectId);
				if (it == m_objectIdToGuid.cend())
					return handled;

				LevelEditorModule::Get().AddToSelection(it->second);
				return handled;
			}
		}

		return Viewport_v2::Handle(event);
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

			uint32_t objectIdCounter = 0;

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

					++objectIdCounter;

					const Core::Mat44f& world = pGo->GetTransform().GetWorldTx();
					ConstBufferPerObject perObject;
					perObject.m_world = world;
					perObject.m_objectId[3] = (objectIdCounter & 0xFF000000) >> 24;
					perObject.m_objectId[2] = (objectIdCounter & 0x00FF0000) >> 16;
					perObject.m_objectId[1] = (objectIdCounter & 0x0000FF00) >> 8;
					perObject.m_objectId[0] = (objectIdCounter & 0x000000FF);

					m_objectIdToGuid[objectIdCounter] = pGo->GetGuid();

					renderModule.SetConstantBuffer(0, sizeof(ConstBufferPerObject), &perObject, 0);

					const Rendering::Mesh* pRenderingMesh = pMesh->GetRenderingMesh();
					renderModule.RenderMesh(*pRenderingMesh);
				}
			}

			m_pObjectIdRenderTarget->EndScene();

			m_pObjectIdRenderTarget->CopyToReabackBuffer(m_pReadbackBuffer);
		}
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

		char buffer[500];
		sprintf_s(buffer, 500, "[%d %d] Object id %d \n", pixelX, pixelY, objectId);
		OutputDebugString(buffer);

		D3D12_RANGE emptyRange{ 0, 0 };
		m_pReadbackBuffer->GetResource()->Unmap(0, &emptyRange);

		return objectId;
	}
}