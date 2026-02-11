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

#include "OsWin/Input.h"

#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"
#include "Systems/GameComponent/Lights/PointLightComponent.h"
#include "Systems/GameComponent/Lights/SpotLightComponent.h"
#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/MaterialRendering.h"

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
		
		//make the list of renderables and lights
		struct Renderable
		{
			const Rendering::Mesh* m_pMesh;
			const Systems::MaterialInstanceAsset* m_pMaterial;
			Core::Mat44f m_worldTx;
		};
		Core::Array<Renderable> allRenderables;
		allRenderables.Reserve(10);

		Core::Array<Rendering::Light> allLights;
		allLights.Reserve(10);
		for (Systems::GameObject* pGo : gameObjects)
		{
			const Core::Array<Systems::GameComponent*>& components = pGo->GetComponents();
			for (const Systems::GameComponent* pComponent : components)
			{
				if (const Systems::DirectionalLightComponent* pLight = pComponent->Cast<Systems::DirectionalLightComponent>())
				{
					uint32_t index = allLights.GetSize();
					allLights.Resize(index + 1);
					Rendering::Light* pGfxLight = &allLights[index];

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f localDirection = pLight->GetDirection();
					localDirection.Set(3, 0);
					Core::Vec4f worldDirection = localDirection * worldTx;

					Core::Float3 direction(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetX(), pLight->GetAmbient().GetY(), pLight->GetAmbient().GetZ());
					Core::Float3 diffuse(pLight->GetDiffuse().GetX(), pLight->GetDiffuse().GetY(), pLight->GetDiffuse().GetZ());
					Core::Float3 specular(pLight->GetSpecular().GetX(), pLight->GetSpecular().GetY(), pLight->GetSpecular().GetZ());
					pGfxLight->MakeDirectionalLight(direction, ambient, diffuse, specular);
				}
				else if (const Systems::PointLightComponent* pLight = pComponent->Cast<Systems::PointLightComponent>())
				{
					uint32_t index = allLights.GetSize();
					allLights.Resize(index + 1);
					Rendering::Light* pGfxLight = &allLights[index];

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f lightPosition = pLight->GetPosition();
					lightPosition.Set(3, 1);
					Core::Vec4f worldPosition = lightPosition * worldTx;

					Core::Float3 position(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetX(), pLight->GetAmbient().GetY(), pLight->GetAmbient().GetZ());
					Core::Float3 diffuse(pLight->GetDiffuse().GetX(), pLight->GetDiffuse().GetY(), pLight->GetDiffuse().GetZ());
					Core::Float3 specular(pLight->GetSpecular().GetX(), pLight->GetSpecular().GetY(), pLight->GetSpecular().GetZ());
					pGfxLight->MakePointLight(position, ambient, diffuse, specular, pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic());
				}
				else if (const Systems::SpotLightComponent* pLight = pComponent->Cast<Systems::SpotLightComponent>())
				{
					uint32_t index = allLights.GetSize();
					allLights.Resize(index + 1);
					Rendering::Light* pGfxLight = &allLights[index];

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f lightPosition = pLight->GetPosition();
					lightPosition.Set(3, 1);
					Core::Vec4f worldPosition = lightPosition * worldTx;

					Core::Vec4f localDirection = pLight->GetDirection();
					localDirection.Set(3, 0);
					Core::Vec4f worldDirection = localDirection * worldTx;

					Core::Float3 position(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ());
					Core::Float3 direction(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetX(), pLight->GetAmbient().GetY(), pLight->GetAmbient().GetZ());
					Core::Float3 diffuse(pLight->GetDiffuse().GetX(), pLight->GetDiffuse().GetY(), pLight->GetDiffuse().GetZ());
					Core::Float3 specular(pLight->GetSpecular().GetX(), pLight->GetSpecular().GetY(), pLight->GetSpecular().GetZ());
					pGfxLight->MakeSpotLight(position, direction, ambient, diffuse, specular, 
						pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic(), 
						pLight->GetCutOff(), pLight->GetOuterCutOff());
				}
				else if (const Systems::StaticMeshComponent* pStaticMesh = pComponent->Cast<Systems::StaticMeshComponent>())
				{
					const Rendering::Mesh* pMesh = nullptr;
					const Systems::MaterialInstanceAsset* pMaterial = nullptr;
					if (pStaticMesh->GetMesh() && pStaticMesh->GetMesh()->GetRenderingMesh())
						pMesh = pStaticMesh->GetMesh()->GetRenderingMesh();

					pMaterial = pStaticMesh->GetMaterialInstance();

					if (pMesh && pMaterial)
					{
						uint32_t index = allRenderables.GetSize();
						allRenderables.PushBack(Renderable());
						Renderable& renderable = allRenderables[index];

						renderable.m_pMesh = pMesh;
						renderable.m_pMaterial = pMaterial;
						renderable.m_worldTx = pStaticMesh->GetOwner()->GetTransform().GetWorldTx();
					}
				}
			}
		}

		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		const DirectX::XMMATRIX view = pCamera->GetViewMatrix();
		const DirectX::XMMATRIX proj = pCamera->GetProjectionMatrix();

		DirectX::XMFLOAT3 cameraPosFloat3;
		DirectX::XMStoreFloat3(&cameraPosFloat3, DirectX::XMVectorNegate(view.r[3]));
		Rendering::PerFrameCBuffer perFrameData(view, proj, cameraPosFloat3);

		//for now add all lights
		Rendering::LightsCBuffer lights;
		uint32_t lightCount = Rendering::LightsCBuffer::MAX_LIGHT_COUNT;
		if (lightCount > allLights.GetSize())
			lightCount = allLights.GetSize();

		for (uint32_t ii = 0; ii < lightCount; ++ii)
		{
			Rendering::Light* pLight = lights.AddLight();
			*pLight = allLights[ii];
		}

		//now render all renderables
		for(const Renderable& renderable : allRenderables)
		{
			DirectX::XMMATRIX wvp = proj * (view * renderable.m_worldTx.m_matrix);

			if (renderable.m_pMaterial->GetBaseMaterial() && renderable.m_pMaterial->GetBaseMaterial()->IsValidForRendering())
			{
				Rendering::PerObjectCBuffer perObjectData(renderable.m_worldTx.m_matrix);

				Systems::MaterialRendering::Bind(*renderable.m_pMaterial, perObjectData, perFrameData, lights);

				renderModule.RenderMesh(*renderable.m_pMesh);
			}
		}

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

				std::map<uint32_t, Core::Guid>::const_iterator it = m_objectIdToGuid.find(objectId);
				if (it == m_objectIdToGuid.cend())
					return handled;

				bool toggle = false;
				if (Os::IsKeyDown(Os::VKeyCodes::Control))
					toggle = true;

				LevelEditorModule& levelEditorModule = LevelEditorModule::Get();
				bool isAlreadySelected = levelEditorModule.IsSelected(it->second);
				if (isAlreadySelected && toggle)
				{
					levelEditorModule.RemoveFromSelection(it->second);
				}
				else if (!isAlreadySelected)
				{
					levelEditorModule.ClearSelection();
					levelEditorModule.AddToSelection(it->second);
				}

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

		//char buffer[500];
		//sprintf_s(buffer, 500, "[%d %d] Object id %d \n", pixelX, pixelY, objectId);
		//OutputDebugString(buffer);

		D3D12_RANGE emptyRange{ 0, 0 };
		m_pReadbackBuffer->GetResource()->Unmap(0, &emptyRange);

		return objectId;
	}
}