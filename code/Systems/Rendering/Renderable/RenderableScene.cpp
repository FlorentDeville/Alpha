/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Rendering/Renderable/RenderableScene.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vectors.h"

#include "Rendering/RenderModule.h"

#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"
#include "Systems/Clock/Clock.h"
#include "Systems/GameComponent/Lights/DirectionalLightComponent.h"
#include "Systems/GameComponent/Lights/PointLightComponent.h"
#include "Systems/GameComponent/Lights/SpotLightComponent.h"
#include "Systems/GameComponent/StaticMeshComponent.h"
#include "Systems/GameComponent/SkyboxComponent.h"
#include "Systems/Objects/GameObject.h"
#include "Systems/Rendering/Renderable/RenderableLight.h"
#include "Systems/Rendering/Renderable/RenderableObject.h"

#include <algorithm>

namespace Systems
{
	static float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition, const Core::Vec4f& cameraPosition, float fov)
	{
		Core::Vec4f dt = cameraPosition - objectPosition;
		float distance = dt.Length();
		float worldSize = (2 * tanf(fov * 0.5f)) * distance;
		const float SCREEN_RATIO = 0.025f;
		float size = SCREEN_RATIO * worldSize;
		return size;
	}

	void PrepareRenderableCamera(const Core::Mat44f& view, const Core::Mat44f& proj, const Core::Vec4f& position, float fov, RenderableScene& scene)
	{
		scene.m_camera.m_view = view;
		scene.m_camera.m_proj = proj;
		scene.m_camera.m_position = position;
		scene.m_camera.m_fov = fov;
	}

	void PrepareRenderableScene(const LevelAsset* pLevel, RenderableScene& scene)
	{
		scene.m_time = Systems::Clock::Get().GetGameTime(); //by default pass the game time

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
						float screenScale = ComputeConstantScreenSizeScale(worldTx.GetT(), scene.m_camera.m_position, scene.m_camera.m_fov);

						float realSize = screenScale * SIZE;
						float realLength = LENGTH * screenScale;

						Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realSize, realLength, realSize, 0));

						Core::Mat44f proxyWorldTx = scale * localTx * worldTx;

						{
							Systems::RenderableObject& renderable = scene.m_opaqueObjects.PushBackDefault();

							renderable.m_pMesh = Rendering::RenderModule::Get().m_pCylinderMesh;
							renderable.m_pMaterial = nullptr;
							renderable.m_worldTx = proxyWorldTx;
							renderable.m_primitiveMesh = true;
							renderable.m_pOwner = pGo;
							renderable.m_view = Systems::RenderView::Game | Systems::RenderView::ObjectId;
						}

						{
							Systems::RenderableObject& renderable = scene.m_opaqueObjects.PushBackDefault();

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
						Systems::RenderableObject& renderable = scene.m_opaqueObjects.PushBackDefault();

						const float SIZE = 0.5f;
						float realSize = ComputeConstantScreenSizeScale(worldPosition, scene.m_camera.m_position, scene.m_camera.m_fov) * SIZE;
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
						//XMVECTOR dxWorldPos = DirectX::XMVectorSet(worldPosition.GetX(), worldPosition.GetY(), worldPosition.GetZ(), worldPosition.GetW());
						//XMVECTOR dxWorldDir = DirectX::XMVectorSet(worldDirection.GetX(), worldDirection.GetY(), worldDirection.GetZ(), worldDirection.GetW());
						//XMMATRIX dxView = DirectX::XMMatrixLookToLH(dxWorldPos, dxWorldDir, DirectX::XMVectorSet(0, 1, 0, 0));
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
						float realSize = ComputeConstantScreenSizeScale(worldPosition, scene.m_camera.m_position, scene.m_camera.m_fov) * SIZE;

						Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realSize, realSize, realSize, 0));

						Core::Mat44f proxyWorldTx = scale * localTx * worldTx;

						Systems::RenderableObject& renderable = scene.m_opaqueObjects.PushBackDefault();

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
						Systems::RenderableObject* pNewRenderable = nullptr;
						if(pMaterial->GetBaseMaterial()->GetBlendMode() == BlendMode::BM_TRANSLUCENT)
							pNewRenderable = &scene.m_translucentObjects.PushBackDefault();
						else
							pNewRenderable = &scene.m_opaqueObjects.PushBackDefault();

						pNewRenderable->m_pMesh = pMesh;
						pNewRenderable->m_pMaterial = pMaterial;
						pNewRenderable->m_worldTx = pStaticMesh->GetOwner()->GetTransform().GetWorldTx();
						pNewRenderable->m_primitiveMesh = false;
						pNewRenderable->m_pOwner = pGo;
						pNewRenderable->m_view = Systems::RenderView::Game | Systems::RenderView::ShadowMap | Systems::RenderView::ObjectId;
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
						Systems::RenderableObject& renderable = scene.m_opaqueObjects.PushBackDefault();

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

		//sort translucent object from the farthest to the closest
		Core::Vec4f& cameraPosition = scene.m_camera.m_position;
		std::sort(scene.m_translucentObjects.begin(), scene.m_translucentObjects.end(), [&cameraPosition](const RenderableObject& obj1, const RenderableObject& obj2)
			{
				float obj1Distance = (obj1.m_worldTx.GetT() - cameraPosition).Length2();
				float obj2Distance = (obj2.m_worldTx.GetT() - cameraPosition).Length2();

				return obj1Distance > obj2Distance;
			});
	}
}