/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/Ichi.h"

#include "Alpha/StateMachine/StateMachine.h"
#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"
#include "Alpha/Objects/Boss/Ichi/States/Ichi_Start.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Particle/ParticleSystem.h"

Ichi::Ichi()
	: BaseClass()
{
	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
		m_engineEffectHandle[ii] = Systems::ParticleEffectHandle();
}

Ichi::~Ichi()
{ }

void Ichi::OnStartGame()
{
	BaseClass::OnStartGame();

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(IchiStateEnum::COUNT);

	m_pStateMachine->AddState(new Ichi_Start(m_pStateMachine, this), IchiStateEnum::START);
	m_pStateMachine->AddState(new Ichi_Phase1_Travel(m_pStateMachine, this), IchiStateEnum::PHASE1_TRAVEL);

	m_pStateMachine->Start(IchiStateEnum::START);
}

void Ichi::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();
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
}

void Ichi::SetCurrentHP(int32_t hp)
{
	BaseClass::SetCurrentHP(hp);
}

void Ichi::SpawnEngineEffects()
{
	Systems::ParticleSystem* pParticleSystem = Systems::GameMgr::Get().GetWorld()->m_pParticleSystem;
	Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;

	const Core::Sid ATTACH_POINTS_NAME[ENGINE_EFFECT_COUNT] =
	{
		CONSTSID("engine0"),
		CONSTSID("engine1"),
		CONSTSID("engine2"),
		CONSTSID("engine3")
	};

	for (uint8_t ii = 0; ii < ENGINE_EFFECT_COUNT; ++ii)
	{
		const Systems::AttachPoint* pAp = m_meshPhase1->FindAttachPoint(ATTACH_POINTS_NAME[ii]);
		if (pAp)
		{
			Core::Mat44f wsTx = pAp->GetLocator().GetMatrix() * GetTransform().GetWorldTx();
			m_engineEffectHandle[ii] = pParticleSystem->SpawnEffect(m_engineEffect.GetPtr(), wsTx, pClock->GetTime());
		}
		else
		{
			Core::LogModule::Get().LogInfo("Can't find attach point for engine effect");
		}
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

Core::Vec4f Ichi::IdleBouncingMotion::ComputeOffset() const
{
	Systems::IClockSubsystem* pClock = Systems::GameMgr::Get().GetWorld()->m_pClock;

	float value = sinf((pClock->GetTime() - m_startTime) * m_frequency) * m_amplitude;
	return Core::Vec4f(0, value, 0, 0);
}
