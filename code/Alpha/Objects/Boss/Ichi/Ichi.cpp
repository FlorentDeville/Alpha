/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Alpha/StateMachine/StateMachine.h"
#include "Alpha/Objects/Boss/Ichi/Motion/IchiMotionState.h"
#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Idle.h"
#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Stop.h"
#include "Alpha/Objects/Boss/Ichi/Motion/Ichi_Motion_Travel.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack1.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Attack2.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Attack1.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Attack2.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Travel.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack1.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Attack2.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Travel.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_To_Phase2.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_To_Phase3.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Dying.h"


#include "Alpha/Objects/Boss/Ichi/States/Ichi_Start.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"

Core::Sid Ichi::ATTACH_POINTS_NAME[Ichi::ENGINE_EFFECT_COUNT] =
{
	Core::MakeSid("engine0"),
	Core::MakeSid("engine1"),
	Core::MakeSid("engine2"),
	Core::MakeSid("engine3")
};

Core::Sid Ichi::ATTACH_POINT_PHASE1_GUN_NAME[Ichi::GUN_PHASE1_COUNT] =
{
	Core::MakeSid("gun0"),
	Core::MakeSid("gun1"),
	Core::MakeSid("gun2"),
	Core::MakeSid("gun3")
};

Ichi::Ichi()
	: BaseClass()
	, m_pMotionStateMachine(nullptr)
{
	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
		m_engineEffectHandle[ii] = Systems::ParticleEffectHandle();
}

Ichi::~Ichi()
{
	delete m_pMotionStateMachine;
	m_pMotionStateMachine = nullptr;
}

void Ichi::OnStartGame()
{
	BaseClass::OnStartGame();

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(IchiStateEnum::COUNT);

	m_pStateMachine->AddState(new Ichi_Start(m_pStateMachine, this), IchiStateEnum::START);
	m_pStateMachine->AddState(new Ichi_Phase1_Travel(m_pStateMachine, this), IchiStateEnum::PHASE1_TRAVEL);
	m_pStateMachine->AddState(new Ichi_Phase1_Attack1(m_pStateMachine, this), IchiStateEnum::PHASE1_ATTACK1);
	m_pStateMachine->AddState(new Ichi_Phase1_Attack2(m_pStateMachine, this), IchiStateEnum::PHASE1_ATTACK2);
	m_pStateMachine->AddState(new Ichi_Phase2_Travel(m_pStateMachine, this), IchiStateEnum::PHASE2_TRAVEL);
	m_pStateMachine->AddState(new Ichi_Phase2_Attack1(m_pStateMachine, this), IchiStateEnum::PHASE2_ATTACK1);
	m_pStateMachine->AddState(new Ichi_Phase2_Attack2(m_pStateMachine, this), IchiStateEnum::PHASE2_ATTACK2);
	m_pStateMachine->AddState(new Ichi_Phase3_Travel(m_pStateMachine, this), IchiStateEnum::PHASE3_TRAVEL);
	m_pStateMachine->AddState(new Ichi_Phase3_Attack1(m_pStateMachine, this), IchiStateEnum::PHASE3_ATTACK1);
	m_pStateMachine->AddState(new Ichi_Phase3_Attack2(m_pStateMachine, this), IchiStateEnum::PHASE3_ATTACK2);
	m_pStateMachine->AddState(new Ichi_Phase1_To_Phase2(m_pStateMachine, this), IchiStateEnum::PHASE1_TO_PHASE2);
	m_pStateMachine->AddState(new Ichi_Phase2_To_Phase3(m_pStateMachine, this), IchiStateEnum::PHASE2_TO_PHASE3);
	m_pStateMachine->AddState(new Ichi_Dying(m_pStateMachine, this), IchiStateEnum::DYING);

	m_pStateMachine->Start(IchiStateEnum::START);
	//m_pStateMachine->Start(IchiStateEnum::PHASE1_ATTACK1);

	m_pMotionStateMachine = new StateMachine();
	m_pMotionStateMachine->Init(IchiMotionState::COUNT);

	m_pMotionStateMachine->AddState(new IchiMotionStop(m_pMotionStateMachine, this), IchiMotionState::STOP);
	m_pMotionStateMachine->AddState(new IchiMotionIdle(m_pMotionStateMachine, this), IchiMotionState::IDLE);
	m_pMotionStateMachine->AddState(new IchiMotionTravel(m_pMotionStateMachine, this), IchiMotionState::TRAVEL);

	m_pMotionStateMachine->Start(IchiMotionState::STOP);

	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
	{
		const Systems::AttachPoint* pAp = m_meshPhase1->FindAttachPoint(ATTACH_POINTS_NAME[ii]);
		if (pAp)
			m_engineAttachPoints[ii] = pAp->GetLocator().GetMatrix();
		else
			Core::LogModule::Get().LogInfo("Can't find attach point for engine");
	}

	for (uint8_t ii = 0; ii < GUN_PHASE1_COUNT; ++ii)
	{
		const Systems::AttachPoint* pAp = m_meshPhase1->FindAttachPoint(ATTACH_POINT_PHASE1_GUN_NAME[ii]);
		if (pAp)
			m_phase1GunsAttachPoints[ii] = pAp->GetLocator().GetMatrix();
		else
			Core::LogModule::Get().LogInfo("Can't find attach point for phase1 gun");
	}
}

void Ichi::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();
	m_pMotionStateMachine->Update();

	Systems::ParticleSystem* pParticleSystem = Systems::GameMgr::Get().GetWorld()->m_pParticleSystem;
	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
	{
		Core::Mat44f wsTx = m_engineAttachPoints[ii] * GetTransform().GetWorldTx();
		pParticleSystem->UpdateTransform(m_engineEffectHandle[ii], wsTx);
	}
}

void Ichi::HandleMessage(const Systems::GameMessage& msg)
{
	BaseClass::HandleMessage(msg);
}

void Ichi::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	KillEngineEffects();

	delete m_pStateMachine;
	m_pStateMachine = nullptr;

	delete m_pMotionStateMachine;
	m_pMotionStateMachine = nullptr;
}

void Ichi::SetCurrentHP(int32_t hp)
{
	BaseClass::SetCurrentHP(hp);
}

void Ichi::EnterPhase1()
{
	Ichi_Phase1_Attack1* pStateP1A1 = m_pStateMachine->GetState<Ichi_Phase1_Attack1>(IchiStateEnum::PHASE1_ATTACK1);
	pStateP1A1->InitWaves();
}

void Ichi::SpawnEngineEffects()
{
	Systems::ParticleSystem* pParticleSystem = Systems::GameMgr::Get().GetWorld()->m_pParticleSystem;
	Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;

	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
	{
		Core::Mat44f wsTx = m_engineAttachPoints[ii] * GetTransform().GetWorldTx();
		m_engineEffectHandle[ii] = pParticleSystem->SpawnEffect(m_engineEffect.GetPtr(), wsTx, pClock->GetTime());
	}
}

void Ichi::KillEngineEffects()
{
	Systems::ParticleSystem* pParticleSystem = Systems::GameMgr::Get().GetWorld()->m_pParticleSystem;

	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
	{
		pParticleSystem->KillEffect(m_engineEffectHandle[ii]);
		m_engineEffectHandle[ii] = Systems::ParticleEffectHandle();
	}
}

void Ichi::GoToMotionState(IchiMotionState::Type newState)
{
	m_pMotionStateMachine->GoTo(newState);
}

void Ichi::GoToMotionStateTravel(const Core::Vec4f& target)
{
	IchiMotionTravel* pState = m_pMotionStateMachine->GetState<IchiMotionTravel>(IchiMotionState::TRAVEL);
	pState->SetTarget(target);

	m_pMotionStateMachine->GoTo(IchiMotionState::TRAVEL);
}

bool Ichi::IsInMotionState(IchiMotionState::Type state) const
{
	return m_pMotionStateMachine->IsInState(state);
}

Systems::MeshAsset* Ichi::GetBulletMesh()
{
	return m_bulletMesh.GetPtr();
}

Systems::MaterialInstanceAsset* Ichi::GetBulletMaterial()
{
	return m_bulletMaterial.GetPtr();
}

const uint8_t Ichi::GetPhase1GunsAttachPointsCount() const
{
	return GUN_PHASE1_COUNT;
}

const Core::Mat44f* Ichi::GetPhase1GunsAttachPoints() const
{
	return m_phase1GunsAttachPoints;
}
