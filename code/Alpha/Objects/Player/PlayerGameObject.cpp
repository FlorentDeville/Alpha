/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Objects/Boss/BaseBoss.h"
#include "Alpha/Objects/Player/States/PlayerStateContext.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"
#include "Alpha/Objects/Player/States/PlayerState_Counter.h"
#include "Alpha/Objects/Player/States/PlayerState_Dash.h"
#include "Alpha/Objects/Player/States/PlayerState_Move.h"
#include "Alpha/Objects/Player/States/PlayerState_PrepareDash.h"
#include "Alpha/Objects/Player/Waves/Player_Wave_Countered.h"
#include "Alpha/StateMachine/StateMachine.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/GameComponent/MeshComponent.h"
#include "Systems/GameComponent/RenderableComponent.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"

#include <cmath>

PlayerGameObject::PlayerGameObject()
	: Systems::GameObject()
	, m_currentHp()
	, m_maxHp()
	, m_speed()
	, m_pStateMachine(nullptr)
	, m_pCounteredBulletWave(nullptr)
	, m_counteredBulletWaveIndex(UINT32_MAX)
	, m_cameraMode(TRACK)
	, m_pDashCircleObject(nullptr)
	, m_pDashCircleMesh(nullptr)
	, m_pDashTargetObject(nullptr)
	, m_pDashTargetMesh(nullptr)
	, m_pPlayerStateContext(nullptr)
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

	m_pPlayerStateContext = new PlayerStateContext();
	m_pPlayerStateContext->m_counterableBulletIndex = UINT32_MAX;

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(PlayerStateEnum::COUNT);
	m_pStateMachine->SetContext(m_pPlayerStateContext);

	m_pStateMachine->AddState(new PlayerState_Move(m_pStateMachine, this), PlayerStateEnum::MOVE);
	m_pStateMachine->AddState(new PlayerState_PrepareDash(m_pStateMachine, this), PlayerStateEnum::PREPARE_DASH);
	m_pStateMachine->AddState(new PlayerState_Dash(m_pStateMachine, this), PlayerStateEnum::DASH);
	m_pStateMachine->AddState(new PlayerState_Counter(m_pStateMachine, this), PlayerStateEnum::COUNTER);

	BaseBoss* pBoss = Systems::GameMgr::Get().FindGameObject<BaseBoss>();
	m_pCounteredBulletWave = new PlayerWaveCountered(m_counteredBulletMesh.GetPtr(), m_counteredBulletMaterial.GetPtr(), this, pBoss);

	BulletSubsystem* pBulletSubsystem = BulletSubsystem::GetSubsystem();
	m_counteredBulletWaveIndex = pBulletSubsystem->AddWave(m_pCounteredBulletWave);
	pBulletSubsystem->InitWave(m_counteredBulletWaveIndex);

	m_cameraMode = TRACK;

	OnStartGame_DashCircle();
	OnGameStart_DashTarget();

	SpawnAndPlayEngineEffect();
}

void PlayerGameObject::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();

	UpdateEngineEffect();
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
		m_pPlayerStateContext->m_counterableBulletIndex = static_cast<uint32_t>(msg.m_param);
	}
	break;

	default:
		break;
	}
}

void PlayerGameObject::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	KillEngineEffect();

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
	m_pDashCircleMesh->SetEnabled(true);
}

void PlayerGameObject::HideDashCircle()
{
	m_pDashCircleMesh->SetEnabled(false);
}

void PlayerGameObject::ShowDashTarget()
{
	m_pDashTargetMesh->SetEnabled(true);
}

void PlayerGameObject::HideDashTarget()
{
	m_pDashTargetMesh->SetEnabled(false);
}

void PlayerGameObject::SetDashTargetRelativePosition(const Core::Vec4f& relPosition)
{
	m_pDashTargetObject->GetTransform().SetLocalTranslation(relPosition);
}

void PlayerGameObject::OnBulletCollision(uint32_t index)
{
	if (m_pStateMachine->GetCurrentState() == PlayerStateEnum::DASH)
		return;

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

void PlayerGameObject::OnGameStart_DashTarget()
{
	Systems::GameMgr& gameMgr = Systems::GameMgr::Get();

	m_pDashTargetObject = gameMgr.FindGameObjectByName("dash_target");
	if (!m_pDashTargetObject)
	{
		Core::LogModule::Get().LogError("Failed to find game object dash_target");
		return;
	}

	m_pDashTargetMesh = m_pDashTargetObject->FindComponent<Systems::MeshComponent>();
	if (!m_pDashTargetMesh)
	{
		Core::LogModule::Get().LogError("Failed to find mesh component dash_target");
		return;
	}

	m_pDashTargetMesh->SetEnabled(false);
}

void PlayerGameObject::SpawnAndPlayEngineEffect()
{
	if (!m_engineEffect.GetPtr())
		return;

	//calculate effect transform
	const Systems::RenderableComponent* pRenderable = FindComponent<Systems::RenderableComponent>();
	if (!pRenderable)
		return;

	if (!pRenderable->GetMesh())
		return;

	const Core::Array<Systems::AttachPoint>& apArray = pRenderable->GetMesh()->GetAttachPoints();
	if (apArray.GetSize() == 0)
		return;

	Core::Mat44f effectTx = apArray[0].GetLocator().GetMatrix() * pRenderable->GetLocalTx().GetMatrix() * GetTransform().GetWorldTx();

	//now spawn and play
	Systems::ParticleSystem* pParticle = Systems::GameMgr::GetParticleSystem();
	Systems::IClockSubsystem* pClock = Systems::GameMgr::GetClock();

	m_engineEffectHandle = pParticle->SpawnEffect(m_engineEffect.GetPtr(), effectTx, pClock->GetTime());
	pParticle->Play(m_engineEffectHandle);
}

void PlayerGameObject::KillEngineEffect()
{
	Systems::ParticleSystem* pParticle = Systems::GameMgr::GetParticleSystem();
	pParticle->KillEffect(m_engineEffectHandle);
	m_engineEffectHandle = Systems::ParticleEffectHandle();
}

void PlayerGameObject::UpdateEngineEffect()
{
	if (!m_engineEffectHandle.IsValid())
		return;

	const Systems::RenderableComponent* pRenderable = FindComponent<Systems::RenderableComponent>();
	const Core::Array<Systems::AttachPoint>& apArray = pRenderable->GetMesh()->GetAttachPoints();
	Core::Mat44f effectTx = apArray[0].GetLocator().GetMatrix() * pRenderable->GetLocalTx().GetMatrix() * GetTransform().GetWorldTx();

	Systems::ParticleSystem* pParticle = Systems::GameMgr::GetParticleSystem();
	pParticle->UpdateTransform(m_engineEffectHandle, effectTx);
}
