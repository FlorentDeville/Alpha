/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"

#include "Core/Helper.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/RenderPassObjectId.h"
#include "Editors/LevelEditor/Widgets/CameraWidget.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"
#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Inputs/InputMgr.h"

#include "OsWin/Input.h"

#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LightsCBuffer.h"
#include "Rendering/ConstantBuffer/PerObjectCBuffer.h"
#include "Rendering/ConstantBuffer/PerFrameCBuffer.h"
#include "Rendering/Device.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"
#include "Systems/GameComponent/Lights/PointLightComponent.h"
#include "Systems/GameComponent/Lights/SpotLightComponent.h"
#include "Systems/GameComponent/SkyboxComponent.h"
#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/MaterialRendering.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"
#include "Systems/Rendering/Renderable/RenderableScene.h"
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

		DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		DirectX::XMVECTOR cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);
		m_pCamera->SetTransform(cameraPosition, cameraEulerAngle);

		m_pGizmoModel = new GizmoModel();
		m_pGizmoWidget = new GizmoWidget();

		m_pGizmoWidget->SetModel(m_pGizmoModel);

		Rendering::TextureId readbackTextureId;
		Rendering::TextureMgr::Get().CreateTexture(&m_pReadbackBuffer, readbackTextureId);
		m_pReadbackBuffer->InitAsReadbackBuffer(width, height, 12);

		m_pRenderPassShadowMaps = new Systems::RenderPassShadowMaps();
		m_pRenderPassObjectId = new RenderPassObjectId(width, height);
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{
		delete m_pCamera;
		delete m_pGizmoWidget;
		delete m_pRenderPassShadowMaps;
		delete m_pRenderPassObjectId;
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

		Core::Vec4f mouseWs = Compute3dPosition(clampedMousePos);

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

			const Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();

			CreateRenderScene(scene);
		}

		//first do the shadow maps
		m_pRenderPassShadowMaps->PreRender(scene);
		m_pRenderPassShadowMaps->Render(scene);
		m_pRenderPassShadowMaps->PostRender(scene);

		//now render the level editor view of the scene.
		m_pRenderTarget->BeginScene();
		
		RenderView_LevelEditor(scene);
		m_pRenderTarget->ClearDepthBuffer();
		m_pGizmoWidget->Render();

		m_pRenderTarget->EndScene();

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

	void LevelEditorViewportWidget::CreateRenderScene(Systems::RenderableScene& scene) const
	{
		Rendering::Camera* pCamera = Rendering::RenderModule::Get().GetCamera();

		scene.m_camera.m_view = pCamera->GetViewMatrix();
		scene.m_camera.m_proj = pCamera->GetProjectionMatrix();
		scene.m_camera.m_position = pCamera->GetPosition();

		scene.m_objects.Reserve(10);
		scene.m_lights.Reserve(10);

		Editors::LevelEditorModule& levelEditorModule = Editors::LevelEditorModule::Get();
		Systems::LevelAsset* pLevel = levelEditorModule.GetCurrentLoadedLevel();
		const Core::Array<Systems::GameObject*>& gameObjects = pLevel->GetGameObjectsArray();

		for (Systems::GameObject* pGo : gameObjects)
		{
			const Core::Array<Systems::GameComponent*>& components = pGo->GetComponents();
			for (const Systems::GameComponent* pComponent : components)
			{
				if (const Systems::DirectionalLightComponent* pLight = pComponent->Cast<Systems::DirectionalLightComponent>())
				{
					Systems::RenderableLight& gfxLight = scene.m_lights.PushBackDefault();

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f localDirection = pLight->GetDirection();
					localDirection.Set(3, 0);
					Core::Vec4f worldDirection = localDirection * worldTx;

					Core::Float3 direction(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetRed(), pLight->GetAmbient().GetGreen(), pLight->GetAmbient().GetBlue());
					Core::Float3 diffuse(pLight->GetDiffuse().GetRed(), pLight->GetDiffuse().GetGreen(), pLight->GetDiffuse().GetBlue());
					Core::Float3 specular(pLight->GetSpecular().GetRed(), pLight->GetSpecular().GetGreen(), pLight->GetSpecular().GetBlue());
					gfxLight.m_cbuffer.MakeDirectionalLight(direction, ambient, diffuse, specular);

					{
						Core::Vec4f lightDir = worldDirection;
						lightDir.Normalize();
						Core::Vec4f up(0, 1, 0, 0);

						if (abs(up.Dot(lightDir)) == 1)
							up = Core::Vec4f(0, 0, 1, 0);

						Core::Mat44f lightView = Core::Mat44f::CreateView(Core::Vec4f(0, 0, 0, 1), lightDir, up);

						//compute the orthogonal projection. To do this I should find all the objects in the frustum and create the orthogonal plane with it.
						//for now just hardcode it for a box with its centre at the orign and a side of 20.
						float side = 15;
						Core::Mat44f lightProjection = Core::Mat44f::CreateOrthographic(-side, side, -side, side, -side, side);

						Core::Mat44f lightSpace = lightView * lightProjection;
						gfxLight.m_lightSpaceTX = lightSpace;
					}

					{
						Core::Mat44f localTx = Core::Mat44f::CreateLookAt(Core::Vec4f(0, 0, 0, 1), localDirection, Core::Vec4f(0, 1, 0, 0));
						Core::Mat44f rot = Core::Mat44f::CreateRotationMatrix(Core::Vec4f(1, 0, 0, 0), -Core::PI_OVER_TWO);
						localTx = rot * localTx;

						const float SIZE = 0.5f;
						const float LENGTH = 3.f;
						float screenScale = ComputeConstantScreenSizeScale(worldTx.GetT());

						float realSize = screenScale * SIZE;
						float realLength = LENGTH * screenScale;

						Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realSize, realLength, realSize, 0));

						Core::Mat44f proxyWorldTx = scale * localTx * worldTx;

						{
							Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

							renderable.m_pMesh = Rendering::RenderModule::Get().m_pCylinderMesh;
							renderable.m_pMaterial = nullptr;
							renderable.m_worldTx = proxyWorldTx;
							renderable.m_primitiveMesh = true;
							renderable.m_pOwner = pGo;
							renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ObjectId;
						}

						{
							Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

							float arrowTipScaleValue = 1.f * screenScale;

							Core::Mat44f arrowTipOffset = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, -realLength * 0.5f - (arrowTipScaleValue * 0.5f), 0, 1));
							Core::Mat44f arrowTipRotation = Core::Mat44f::CreateRotationMatrixFromEulerAngles(Core::Vec4f(Core::PI, 0, 0, 0));
							Core::Mat44f arrowTipScale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(arrowTipScaleValue, arrowTipScaleValue, arrowTipScaleValue, 0));

							renderable.m_pMesh = Rendering::RenderModule::Get().m_pConeMesh;
							renderable.m_pMaterial = nullptr;
							renderable.m_worldTx = arrowTipScale * arrowTipRotation * arrowTipOffset * localTx * worldTx;
							renderable.m_primitiveMesh = true;
							renderable.m_pOwner = pGo;
							renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ObjectId;
						}
					}
				}
				else if (const Systems::PointLightComponent* pLight = pComponent->Cast<Systems::PointLightComponent>())
				{
					Systems::RenderableLight& gfxLight = scene.m_lights.PushBackDefault();

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f lightPosition = pLight->GetPosition();
					lightPosition.Set(3, 1);
					Core::Vec4f worldPosition = lightPosition * worldTx;

					Core::Float3 position(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetRed(), pLight->GetAmbient().GetGreen(), pLight->GetAmbient().GetBlue());
					Core::Float3 diffuse(pLight->GetDiffuse().GetRed(), pLight->GetDiffuse().GetGreen(), pLight->GetDiffuse().GetBlue());
					Core::Float3 specular(pLight->GetSpecular().GetRed(), pLight->GetSpecular().GetGreen(), pLight->GetSpecular().GetBlue());
					gfxLight.m_cbuffer.MakePointLight(position, ambient, diffuse, specular, pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic());

					{
						Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

						const float SIZE = 0.5f;
						float realSize = ComputeConstantScreenSizeScale(worldPosition) * SIZE;
						Core::Sqt sqt;
						sqt.SetTranslation(worldPosition);
						sqt.SetScale(Core::Vec4f(realSize, realSize, realSize, 0));
						Core::Mat44f proxyWorldTx = Core::Mat44f::CreateTransformMatrix(sqt);
						renderable.m_pMesh = Rendering::RenderModule::Get().m_pSphereMesh;
						renderable.m_pMaterial = nullptr;
						renderable.m_worldTx = proxyWorldTx;
						renderable.m_primitiveMesh = true;
						renderable.m_pOwner = pGo;
						renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ObjectId;
					}
				}
				else if (const Systems::SpotLightComponent* pLight = pComponent->Cast<Systems::SpotLightComponent>())
				{
					Systems::RenderableLight& gfxLight = scene.m_lights.PushBackDefault();

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f lightPosition = pLight->GetPosition();
					lightPosition.Set(3, 1);
					Core::Vec4f worldPosition = lightPosition * worldTx;

					Core::Vec4f localDirection = pLight->GetDirection();
					localDirection.Set(3, 0);
					Core::Vec4f worldDirection = localDirection * worldTx;

					Core::Float3 position(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ());
					Core::Float3 direction(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetRed(), pLight->GetAmbient().GetGreen(), pLight->GetAmbient().GetBlue());
					Core::Float3 diffuse(pLight->GetDiffuse().GetRed(), pLight->GetDiffuse().GetGreen(), pLight->GetDiffuse().GetBlue());
					Core::Float3 specular(pLight->GetSpecular().GetRed(), pLight->GetSpecular().GetGreen(), pLight->GetSpecular().GetBlue());
					gfxLight.m_cbuffer.MakeSpotLight(position, direction, ambient, diffuse, specular,
						pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic(),
						pLight->GetCutOff(), pLight->GetOuterCutOff());

					{
						XMVECTOR dxWorldPos = DirectX::XMVectorSet(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ(), worldPosition.GetW());
						XMVECTOR dxWorldDir = DirectX::XMVectorSet(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ(), worldDirection.GetW());
						XMMATRIX dxView = DirectX::XMMatrixLookToLH(dxWorldPos, dxWorldDir, DirectX::XMVectorSet(0, 1, 0, 0));
						Core::Mat44f lightView = Core::Mat44f::CreateView(worldPosition, worldDirection, Core::Vec4f(0, 1, 0, 0));
						Core::Mat44f lightProjection = Core::Mat44f::CreatePerspective(pLight->GetOuterCutOff() * 2, 1.f, 0.1f, 100);

						Core::Mat44f lightSpace = lightView * lightProjection;
						gfxLight.m_lightSpaceTX = lightSpace;
					}

					{
						Core::Mat44f localTx = Core::Mat44f::CreateLookAt(lightPosition, localDirection, Core::Vec4f(0, 1, 0, 0));
						Core::Mat44f rot = Core::Mat44f::CreateRotationMatrix(Core::Vec4f(1, 0, 0, 0), -Core::PI_OVER_TWO);
						localTx = rot * localTx;

						Core::Vec4f x = localTx.GetX();
						x.Normalize();
						Core::Vec4f y = localTx.GetY();
						y.Normalize();
						Core::Vec4f z = localTx.GetZ();
						z.Normalize();

						const float SIZE = 1.f;
						float realSize = ComputeConstantScreenSizeScale(worldPosition) * SIZE;
						
						Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realSize, realSize, realSize, 0));
						
						Core::Mat44f proxyWorldTx = scale * localTx * worldTx;

						Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

						renderable.m_pMesh = Rendering::RenderModule::Get().m_pConeMesh;
						renderable.m_pMaterial = nullptr;
						renderable.m_worldTx = proxyWorldTx;
						renderable.m_primitiveMesh = true;
						renderable.m_pOwner = pGo;
						renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ObjectId;
					}

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
						Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

						renderable.m_pMesh = pMesh;
						renderable.m_pMaterial = pMaterial;
						renderable.m_worldTx = pStaticMesh->GetOwner()->GetTransform().GetWorldTx();
						renderable.m_primitiveMesh = false;
						renderable.m_pOwner = pGo;
						renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap | Systems::RenderView::ObjectId;
					}
				}
				else if (const Systems::SkyboxComponent* pSkybox = pComponent->Cast<Systems::SkyboxComponent>())
				{
					const Rendering::Mesh* pMesh = nullptr;
					const Systems::MaterialInstanceAsset* pMaterial = nullptr;
					if (pSkybox->GetMesh() && pSkybox->GetMesh()->GetRenderingMesh())
						pMesh = pSkybox->GetMesh()->GetRenderingMesh();

					pMaterial = pSkybox->GetMaterialInstance();

					if (pMesh && pMaterial)
					{
						Systems::RenderableObject& renderable = scene.m_objects.PushBackDefault();

						renderable.m_pMesh = pMesh;
						renderable.m_pMaterial = pMaterial;
						//renderable.m_worldTx = pSkybox->GetOwner()->GetTransform().GetWorldTx(); //no need to set the world matrix for a skybox
						renderable.m_primitiveMesh = false;
						renderable.m_pOwner = pGo;
						renderable.m_view = Systems::RenderView::Game;
					}
				}
			}
		}
	}

	void LevelEditorViewportWidget::RenderView_LevelEditor(const Systems::RenderableScene& scene) const
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		const Core::Mat44f view = pCamera->GetViewMatrix();
		const Core::Mat44f proj = pCamera->GetProjectionMatrix();

		const Core::Vec4f& pos = pCamera->GetPosition();
		Core::Float3 cameraPosFloat3(pos.GetX(), pos.GetY(), pos.GetZ());
		Rendering::PerFrameCBuffer perFrameData(view, proj, cameraPosFloat3);

		//bind the shadow map
		Rendering::RenderTarget** pShadowRenderTargets = m_pRenderPassShadowMaps->GetRenderTargets();
		for (uint32_t ii = 0; ii < m_pRenderPassShadowMaps->GetRenderTargetsCount(); ++ii)
		{
			Rendering::Texture* pShadowMapTexture = pShadowRenderTargets[ii]->GetColorTexture();
			pShadowMapTexture->TransitionToShaderResource();
		}

		//for now add all lights
		Rendering::LightsArrayCBuffer lightsConstBuffer;
		Core::Mat44f lightSpace[Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT];

		uint32_t lightCount = min(scene.m_lights.GetSize(), Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);
		for (uint32_t ii = 0; ii < lightCount; ++ii)
		{
			Rendering::LightCBuffer* pLight = lightsConstBuffer.AddLight();
			*pLight = scene.m_lights[ii].m_cbuffer;
			lightSpace[ii] = scene.m_lights[ii].m_lightSpaceTX;
		}

		//now render all renderables
		for (const Systems::RenderableObject& renderable : scene.m_objects)
		{
			if (!(renderable.m_view & Systems::RenderView::Game))
				continue;

			if (renderable.m_primitiveMesh)
			{
				renderModule.RenderBaseShape(renderable.m_pMesh, renderable.m_worldTx.m_matrix, Core::Float4(1, 1, 1, 1));
			}
			else
			{
				const Systems::MaterialAsset* pMaterial = renderable.m_pMaterial->GetBaseMaterial();
				if (pMaterial && pMaterial->IsValidForRendering())
				{
					Rendering::PerObjectCBuffer perObjectData;
					perObjectData.m_world = Core::Mat44f(renderable.m_worldTx.m_matrix);
					memcpy(perObjectData.m_lightSpaceMatrix, lightSpace, sizeof(Core::Mat44f) * Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT);

					Systems::MaterialRendering::Bind(*renderable.m_pMaterial, perObjectData, perFrameData, lightsConstBuffer);

					int32_t shadowMapsRootSigIndex = pMaterial->GetShadowMapsRootSigIndex();
					if (shadowMapsRootSigIndex != -1)
					{
						Rendering::DescriptorHeap* pDescriptionHeap = m_pRenderPassShadowMaps->GetSrvHeap();
						ID3D12DescriptorHeap* pDescriptorHeap[] = { pDescriptionHeap->GetHeap() };
						renderModule.GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
						renderModule.GetRenderCommandList()->SetGraphicsRootDescriptorTable(shadowMapsRootSigIndex, pDescriptionHeap->GetHeap()->GetGPUDescriptorHandleForHeapStart());
					}

					renderModule.RenderMesh(*renderable.m_pMesh);
				}
			}
		}
	}

	float LevelEditorViewportWidget::ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const
	{
		const LevelEditorModule& levelEditor = LevelEditorModule::Get();
		const Core::Mat44f& camera = levelEditor.GetCameraWs();
		float fov = levelEditor.GetFovRad();

		Core::Vec4f dt = camera.GetT() - objectPosition;
		float distance = dt.Length();
		float worldSize = (2 * tanf(fov * 0.5f)) * distance;
		const float SCREEN_RATIO = 0.025f;
		float size = SCREEN_RATIO * worldSize;
		return size;
	}
}