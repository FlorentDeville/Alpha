/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/LevelEditorViewportWidget.h"

#include "Core/Math/Constants.h"
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

		m_pShadowRenderTarget = new Rendering::RenderTarget(1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, Core::Vec4f(0, 0, 0, 0));
	}

	LevelEditorViewportWidget::~LevelEditorViewportWidget()
	{
		delete m_pCamera;
		delete m_pGizmoWidget;
		delete m_pObjectIdRenderTarget;
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
		//compute the camera
		m_pCamera->Render(m_aspectRatio);

		//create the render scene
		Core::Array<Renderable> allRenderables;
		Core::Array<Rendering::LightCBuffer> allLights;
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

			CreateRenderScene(allRenderables, allLights);
		}

		//first do the shadow maps
		RenderView_ShadowMap(allRenderables, allLights);

		//now render the level editor view of the scene.
		m_pRenderTarget->BeginScene();
		
		RenderView_LevelEditor(allRenderables, allLights);
		m_pRenderTarget->ClearDepthBuffer();
		m_pGizmoWidget->Render();

		m_pRenderTarget->EndScene();

		//now render the object ids view
		m_pObjectIdRenderTarget->BeginScene();
		RenderView_ObjectId(allRenderables);
		m_pObjectIdRenderTarget->EndScene();

		m_pObjectIdRenderTarget->CopyToReabackBuffer(m_pReadbackBuffer);
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

	void LevelEditorViewportWidget::CreateRenderScene(Core::Array<Renderable>& renderables, Core::Array<Rendering::LightCBuffer>& lights) const
	{
		renderables.Reserve(10);
		lights.Reserve(10);

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
					uint32_t index = lights.GetSize();
					lights.Resize(index + 1);
					Rendering::LightCBuffer* pGfxLight = &lights[index];

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f localDirection = pLight->GetDirection();
					localDirection.Set(3, 0);
					Core::Vec4f worldDirection = localDirection * worldTx;

					Core::Float3 direction(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetRed(), pLight->GetAmbient().GetGreen(), pLight->GetAmbient().GetBlue());
					Core::Float3 diffuse(pLight->GetDiffuse().GetRed(), pLight->GetDiffuse().GetGreen(), pLight->GetDiffuse().GetBlue());
					Core::Float3 specular(pLight->GetSpecular().GetRed(), pLight->GetSpecular().GetGreen(), pLight->GetSpecular().GetBlue());
					pGfxLight->MakeDirectionalLight(direction, ambient, diffuse, specular);

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
							uint32_t index = renderables.GetSize();
							renderables.PushBack(Renderable());
							Renderable& renderable = renderables[index];

							renderable.m_pMesh = Rendering::RenderModule::Get().m_pCylinderMesh;
							renderable.m_pMaterial = nullptr;
							renderable.m_worldTx = proxyWorldTx;
							renderable.m_primitiveMesh = true;
							renderable.m_pOwner = pGo;
						}

						{
							uint32_t index = renderables.GetSize();
							renderables.PushBack(Renderable());
							Renderable& renderable = renderables[index];

							float arrowTipScaleValue = 1.f * screenScale;

							Core::Mat44f arrowTipOffset = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0, -realLength * 0.5f - (arrowTipScaleValue * 0.5f), 0, 1));
							Core::Mat44f arrowTipRotation = Core::Mat44f::CreateRotationMatrixFromEulerAngles(Core::Vec4f(Core::PI, 0, 0, 0));
							Core::Mat44f arrowTipScale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(arrowTipScaleValue, arrowTipScaleValue, arrowTipScaleValue, 0));

							renderable.m_pMesh = Rendering::RenderModule::Get().m_pConeMesh;
							renderable.m_pMaterial = nullptr;
							renderable.m_worldTx = arrowTipScale * arrowTipRotation * arrowTipOffset * localTx * worldTx;
							renderable.m_primitiveMesh = true;
							renderable.m_pOwner = pGo;
						}
					}
				}
				else if (const Systems::PointLightComponent* pLight = pComponent->Cast<Systems::PointLightComponent>())
				{
					uint32_t index = lights.GetSize();
					lights.Resize(index + 1);
					Rendering::LightCBuffer* pGfxLight = &lights[index];

					Core::Mat44f worldTx = pLight->GetOwner()->GetTransform().GetWorldTx();
					Core::Vec4f lightPosition = pLight->GetPosition();
					lightPosition.Set(3, 1);
					Core::Vec4f worldPosition = lightPosition * worldTx;

					Core::Float3 position(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ());
					Core::Float3 ambient(pLight->GetAmbient().GetRed(), pLight->GetAmbient().GetGreen(), pLight->GetAmbient().GetBlue());
					Core::Float3 diffuse(pLight->GetDiffuse().GetRed(), pLight->GetDiffuse().GetGreen(), pLight->GetDiffuse().GetBlue());
					Core::Float3 specular(pLight->GetSpecular().GetRed(), pLight->GetSpecular().GetGreen(), pLight->GetSpecular().GetBlue());
					pGfxLight->MakePointLight(position, ambient, diffuse, specular, pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic());

					{
						uint32_t index = renderables.GetSize();
						renderables.PushBack(Renderable());
						Renderable& renderable = renderables[index];

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
					}
				}
				else if (const Systems::SpotLightComponent* pLight = pComponent->Cast<Systems::SpotLightComponent>())
				{
					uint32_t index = lights.GetSize();
					lights.Resize(index + 1);
					Rendering::LightCBuffer* pGfxLight = &lights[index];

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
					pGfxLight->MakeSpotLight(position, direction, ambient, diffuse, specular,
						pLight->GetConstant(), pLight->GetLinear(), pLight->GetQuadratic(),
						pLight->GetCutOff(), pLight->GetOuterCutOff());

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

						uint32_t index = renderables.GetSize();
						renderables.PushBack(Renderable());
						Renderable& renderable = renderables[index];

						renderable.m_pMesh = Rendering::RenderModule::Get().m_pConeMesh;
						renderable.m_pMaterial = nullptr;
						renderable.m_worldTx = proxyWorldTx;
						renderable.m_primitiveMesh = true;
						renderable.m_pOwner = pGo;
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
						uint32_t index = renderables.GetSize();
						renderables.PushBack(Renderable());
						Renderable& renderable = renderables[index];

						renderable.m_pMesh = pMesh;
						renderable.m_pMaterial = pMaterial;
						renderable.m_worldTx = pStaticMesh->GetOwner()->GetTransform().GetWorldTx();
						renderable.m_primitiveMesh = false;
						renderable.m_pOwner = pGo;
					}
				}
			}
		}
	}

	void LevelEditorViewportWidget::RenderView_LevelEditor(Core::Array<Renderable>& renderables, Core::Array<Rendering::LightCBuffer>& lights) const
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		const DirectX::XMMATRIX view = pCamera->GetViewMatrix();
		const DirectX::XMMATRIX proj = pCamera->GetProjectionMatrix();

		DirectX::XMFLOAT3 cameraPosFloat3;
		DirectX::XMStoreFloat3(&cameraPosFloat3, DirectX::XMVectorNegate(view.r[3]));
		Rendering::PerFrameCBuffer perFrameData(view, proj, cameraPosFloat3);

		//for now add all lights
		Rendering::LightsArrayCBuffer lightsConstBuffer;
		uint32_t lightCount = Rendering::LightsArrayCBuffer::MAX_LIGHT_COUNT;
		if (lightCount > lights.GetSize())
			lightCount = lights.GetSize();

		for (uint32_t ii = 0; ii < lightCount; ++ii)
		{
			Rendering::LightCBuffer* pLight = lightsConstBuffer.AddLight();
			*pLight = lights[ii];
		}

		//now render all renderables
		for (const Renderable& renderable : renderables)
		{
			if (renderable.m_primitiveMesh)
			{
				renderModule.RenderBaseShape(renderable.m_pMesh, renderable.m_worldTx.m_matrix, Core::Float4(1, 1, 1, 1));
			}
			else
			{
				DirectX::XMMATRIX wvp = proj * (view * renderable.m_worldTx.m_matrix);

				if (renderable.m_pMaterial->GetBaseMaterial() && renderable.m_pMaterial->GetBaseMaterial()->IsValidForRendering())
				{
					Rendering::PerObjectCBuffer perObjectData(renderable.m_worldTx.m_matrix);

					Systems::MaterialRendering::Bind(*renderable.m_pMaterial, perObjectData, perFrameData, lightsConstBuffer);

					renderModule.RenderMesh(*renderable.m_pMesh);
				}
			}
		}
	}

	void LevelEditorViewportWidget::RenderView_ObjectId(Core::Array<Renderable>& renderables)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();

		ConstBufferPerFrame perFrame;
		perFrame.m_view.m_matrix = renderModule.GetConstCamera()->GetViewMatrix();
		perFrame.m_proj.m_matrix = renderModule.GetConstCamera()->GetProjectionMatrix();

		Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::PipelineStateId objectIdsPsoId = widgetMgr.GetObjectIdsPsoId();
		Rendering::PipelineState* pPso = Rendering::PipelineStateMgr::Get().GetPipelineState(objectIdsPsoId);
		renderModule.BindMaterial(*pPso);

		renderModule.SetConstantBuffer(1, sizeof(ConstBufferPerFrame), &perFrame, 0);

		uint32_t objectIdCounter = 0;

		for (const Renderable& renderable : renderables)
		{
			++objectIdCounter;

			const Core::Mat44f& world = renderable.m_worldTx;
			ConstBufferPerObject perObject;
			perObject.m_world = world;
			perObject.m_objectId[3] = (objectIdCounter & 0xFF000000) >> 24;
			perObject.m_objectId[2] = (objectIdCounter & 0x00FF0000) >> 16;
			perObject.m_objectId[1] = (objectIdCounter & 0x0000FF00) >> 8;
			perObject.m_objectId[0] = (objectIdCounter & 0x000000FF);

			m_objectIdToGuid[objectIdCounter] = renderable.m_pOwner->GetGuid();

			renderModule.SetConstantBuffer(0, sizeof(ConstBufferPerObject), &perObject, 0);

			const Rendering::Mesh* pRenderingMesh = renderable.m_pMesh;
			renderModule.RenderMesh(*pRenderingMesh);
		}
	}

	void LevelEditorViewportWidget::RenderView_ShadowMap(Core::Array<Renderable>& renderables, Core::Array<Rendering::LightCBuffer>& lights) const
	{
		//find the directional light
		const Rendering::LightCBuffer* pDirLight = nullptr;
		for (const Rendering::LightCBuffer& light : lights)
		{
			if (light.m_type == Rendering::LightType::Directional)
			{
				pDirLight = &light;
				break;
			}
		}

		if (!pDirLight)
			return;

		Core::Vec4f lightDir(pDirLight->m_direction.x, pDirLight->m_direction.y, pDirLight->m_direction.z, 0);
		Core::Mat44f lightView = Core::Mat44f::CreateView(Core::Vec4f(0, 0, 0, 1), lightDir, Core::Vec4f(0, 1, 0, 0));

		//compute the orthogonal projection. To do this I should find all the objects in the frustum and create the orthogonal plane with it.
		//for now just hardcode it for a box with its centre at the orign and a side of 20.
		float side = 40;
		Core::Mat44f lightProjection = Core::Mat44f::CreateOrtho(-side, side, -side, side, -side, side);

		Core::Mat44f lightSpace = lightProjection * lightView;

		Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		Rendering::PipelineStateId shadowMapPsoId = widgetMgr.GetShadowMapPsoId();
		Rendering::PipelineState* pPso = Rendering::PipelineStateMgr::Get().GetPipelineState(shadowMapPsoId);

		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		renderModule.BindMaterial(*pPso);

		//bind the light space matrix
		renderModule.SetConstantBuffer(1, sizeof(Core::Mat44f), &lightSpace, 0);

		m_pShadowRenderTarget->BeginScene();

		//loop through renderable
		for (const Renderable& renderable : renderables)
		{
			renderModule.SetConstantBuffer(0, sizeof(Core::Mat44f), &renderable.m_worldTx, 0);
			renderModule.RenderMesh(*renderable.m_pMesh);
		}

		m_pShadowRenderTarget->EndScene();
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