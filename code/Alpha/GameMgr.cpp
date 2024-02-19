/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "GameMgr.h"

#include "Components/MeshComponent.h"
#include "Entities/CameraEntity.h"
#include "Entities/GameEntity.h"
#include "Entities/PlayerEntity.h"

#include "Systems/ComponentId.h"
#include "Systems/TransformComponent.h"
#include "Systems/RenderingComponent.h"
#include "Systems/Scene.h"

GameMgr::GameMgr()
	: m_entities()
	, m_pCurrentCamera()
{
	m_pScene = new Systems::Scene();
}

GameMgr::~GameMgr()
{
	for (GameEntity* pEntity : m_entities)
		delete pEntity;

	m_entities.clear();

	m_pCurrentCamera = nullptr;
}

void GameMgr::Init()
{
	//register the components
	int transformComponentId = Systems::GetComponentId<Systems::TransformComponent>();
	int renderingComponentId = Systems::GetComponentId<Systems::RenderingComponent>();

	m_pScene->InitComponent(transformComponentId, sizeof(Systems::TransformComponent));
	m_pScene->InitComponent(renderingComponentId, sizeof(Systems::RenderingComponent));
}

void GameMgr::Release()
{}

void GameMgr::CreatePlayerEntity(Rendering::MeshId meshId, Rendering::MaterialId materialId)
{
	PlayerEntity* pEntity = new PlayerEntity();

	MeshComponent* pMesh = new MeshComponent(pEntity, meshId, materialId);
	pEntity->AddGameComponent(pMesh);

	DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(0, 1, 0);
	pEntity->SetTransform(transform);

	m_entities.push_back(pEntity);
}

void GameMgr::CreateCameraEntity(float aspectRatio)
{
	CameraEntity* pCamera = new CameraEntity(aspectRatio);
	m_entities.push_back(pCamera);
	m_pCurrentCamera = pCamera;
}

void GameMgr::CreateBackgroundEntity(Rendering::MeshId meshId, Rendering::MaterialId materialId)
{
	LocatorEntity* pEntity = new LocatorEntity();

	MeshComponent* pMesh = new MeshComponent(pEntity, meshId, materialId);
	pEntity->AddGameComponent(pMesh);

	DirectX::XMMATRIX transform = DirectX::XMMatrixScaling(100, 1, 100);
	pEntity->SetTransform(transform);

	m_entities.push_back(pEntity);
}

void GameMgr::Update()
{
	for (GameEntity* pEntity : m_entities)
		pEntity->Update();
}

void GameMgr::Render()
{
	for (GameEntity* pEntity : m_entities)
		pEntity->Render();
}

const DirectX::XMMATRIX& GameMgr::GetView() const
{
	return m_pCurrentCamera->GetView();
}

const DirectX::XMMATRIX& GameMgr::GetProjection() const
{
	return m_pCurrentCamera->GetProjection();
}
