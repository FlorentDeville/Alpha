/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Objects/Boss/BaseBoss.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"
#include "Alpha/Objects/Player/States/PlayerState_Dash.h"
#include "Alpha/Objects/Player/States/PlayerState_Move.h"
#include "Alpha/Objects/Player/Waves/Player_Wave_Countered.h"
#include "Alpha/StateMachine/StateMachine.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/GameComponent/MeshComponent.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"

#include <cmath>

PlayerGameObject::PlayerGameObject()
	: Systems::GameObject()
	, m_currentHp()
	, m_maxHp()
	, m_speed()
	, m_pStateMachine(nullptr)
	, m_pStateMove(nullptr)
	, m_pCounteredBulletWave(nullptr)
	, m_counteredBulletWaveIndex(UINT32_MAX)
	, m_cameraMode(TRACK)
	, m_pDashCircleObject(nullptr)
	, m_pDashCircleMesh(nullptr)
{
	m_pCamera = new Rendering::Camera();
}

PlayerGameObject::~PlayerGameObject()
{
	delete m_pCamera;
}

void PlayerGameObject::OnStartGame()
{
	BaseClass::OnStartGame();

	Systems::TransformComponent& transform = GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	Systems::GameMgr& gameMgr = Systems::GameMgr::Get();
	Systems::GameContext* pContext = gameMgr.GetWorld();
	pContext->m_pCameraSubsystem->PushCamera(m_pCamera);
	m_pCamera->SetLookAt(localTx.GetT() + m_cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
	m_pCamera->SetProjection(45 * Core::PI_OVER_180, 1920.f / 1080.f, 0.1f, 1000);

	m_currentHp = m_maxHp;

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(PlayerStateEnum::COUNT);

	m_pStateMove = new PlayerState_Move(m_pStateMachine, this);
	m_pStateMachine->AddState(m_pStateMove, PlayerStateEnum::MOVE);

	PlayerState_Dash* pStateDash = new PlayerState_Dash(m_pStateMachine, this);
	m_pStateMachine->AddState(pStateDash, PlayerStateEnum::DASH);

	BaseBoss* pBoss = Systems::GameMgr::Get().FindGameObject<BaseBoss>();
	m_pCounteredBulletWave = new PlayerWaveCountered(m_counteredBulletMesh.GetPtr(), m_counteredBulletMaterial.GetPtr(), this, pBoss);

	BulletSubsystem* pBulletSubsystem = BulletSubsystem::GetSubsystem();
	m_counteredBulletWaveIndex = pBulletSubsystem->AddWave(m_pCounteredBulletWave);
	pBulletSubsystem->InitWave(m_counteredBulletWaveIndex);

	m_cameraMode = TRACK;

	OnStartGame_DashCircle();
}

void PlayerGameObject::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();
}

void PlayerGameObject::PostUpdate()
{
	UpdateCamera();
}

void PlayerGameObject::HandleMessage(const Systems::GameMessage& msg)
{
	switch (msg.m_id)
	{
	case SID("bullet_collision"):
	{
		OnBulletCollision(static_cast<uint32_t>(msg.m_param));
	}
	break;

	case SID("counter_bullet_collision"):
	{
		Core::LogModule::Get().LogInfo("Message counter_bullet_collision received");
		m_pStateMove->SetCounterBulletIndex(static_cast<uint32_t>(msg.m_param));
	}
	break;

	default:
		break;
	}
}

void PlayerGameObject::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	Systems::GameContext* pContext = Systems::GameMgr::Get().GetWorld();
	pContext->m_pCameraSubsystem->PopCamera();

	delete m_pStateMachine;
	m_pStateMachine = nullptr;

	BulletSubsystem* pBulletSubsystem = BulletSubsystem::GetSubsystem();
	pBulletSubsystem->DestroyWave(m_counteredBulletWaveIndex);
	pBulletSubsystem->RemoveWave(m_counteredBulletWaveIndex);
	m_counteredBulletWaveIndex = UINT32_MAX;

	delete m_pCounteredBulletWave;
	m_pCounteredBulletWave = nullptr;
}

float PlayerGameObject::GetSpeed() const
{
	return m_speed;
}

void PlayerGameObject::SpawnCounteredBullet(const Core::Vec4f& startPosition)
{
	m_pCounteredBulletWave->SpawnSingleBullet(startPosition);
}

void PlayerGameObject::SetCameraModeTracking()
{
	m_cameraMode = TRACK;
}

void PlayerGameObject::SetCameraModeLocked()
{
	m_cameraMode = LOCK;
}

void PlayerGameObject::ShowDashCircle()
{
	m_pDashCircleObject->GetTransform().SetLocalTranslation(GetTransform().GetWorldPosition());
	m_pDashCircleMesh->SetEnabled(true);
}

void PlayerGameObject::HideDashCircle()
{
	m_pDashCircleMesh->SetEnabled(false);
}

void PlayerGameObject::OnBulletCollision(uint32_t index)
{
	//if (m_pStateMachine->GetCurrentState() == PlayerStateEnum::DASH)
	//	return;

	BulletSubsystem* pBullets = BulletSubsystem::GetSubsystem();
	pBullets->KillBullet(index);

	const uint32_t DAMAGE = 5;
	m_currentHp -= DAMAGE;
	if (m_currentHp > m_maxHp) m_currentHp = 0;

	Systems::UIBaseComponent* pCurrentHpUIComp = m_currentHealthComp.FindComponent(this);
	Systems::UIBaseComponent* pTotalHpUIComp = m_totalHealthComp.FindComponent(this);
	
	float totalWidth = pTotalHpUIComp->GetSize().x;
	float currentWidth = totalWidth / m_maxHp * m_currentHp;

	Core::Float2 size = pCurrentHpUIComp->GetSize();
	size.x = currentWidth;
	pCurrentHpUIComp->SetSize(size);

	Core::Float2 position = pCurrentHpUIComp->GetPosition();
	position.x = pTotalHpUIComp->GetPosition().x - ((totalWidth - currentWidth) / 2);
	pCurrentHpUIComp->SetPosition(position);
}

void PlayerGameObject::UpdateCamera()
{
	switch (m_cameraMode)
	{
	case TRACK:
	{
		const BaseBoss* pBoss = Systems::GameMgr::Get().FindGameObject<BaseBoss>();
		Core::Vec4f bossPos = pBoss->GetTransform().GetWorldTx().GetT();

		const Core::Vec4f& playerPos = GetTransform().GetWorldTx().GetT();

		//calculate the midpoint
		Core::Vec4f targetPos = (bossPos + playerPos) * 0.5f;

		//calculate the distance
		float halfDistance = (bossPos - playerPos).Length() * 0.5f;

		//calculate distance using the vertical axis
		float phi = m_pCamera->GetFOV() / m_pCamera->GetAspectRatio();
		float halfPhi = phi * 0.5f;
		float cameraDistance = halfDistance / std::tan(halfPhi);

		//clamp
		const float MIN_DISTANCE = 70;
		const float MAX_DISTANCE = 100;
		cameraDistance = cameraDistance < MIN_DISTANCE ? MIN_DISTANCE : (cameraDistance > MAX_DISTANCE ? MAX_DISTANCE : cameraDistance);

		//compute camera position
		const float MARGIN = 5.f;
		m_cameraOffset.Normalize();
		Core::Vec4f cameraPosition = targetPos + m_cameraOffset * (cameraDistance + MARGIN);

		m_pCamera->SetLookAt(cameraPosition, targetPos, Core::Vec4f(0, 1, 0, 0));
	}
	break;

	case LOCK:
	{}
	break;
	}
}

void PlayerGameObject::OnStartGame_DashCircle()
{
	Systems::GameMgr& gameMgr = Systems::GameMgr::Get();

	m_pDashCircleObject = gameMgr.FindGameObjectByName("dash_circle");
	if (!m_pDashCircleObject)
	{
		Core::LogModule::Get().LogError("Failed to find game object dash_circle");
		return;
	}
	
	m_pDashCircleMesh = m_pDashCircleObject->FindComponent<Systems::MeshComponent>();
	if (!m_pDashCircleMesh)
	{
		Core::LogModule::Get().LogError("Failed to find mesh component dash_circle");
		return;
	}
	
	m_pDashCircleMesh->SetEnabled(false);
	
}
