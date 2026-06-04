/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Proto/States/BossState_Phase1_Travel.h"

#include "Alpha/Objects/Boss/Proto/States/BossStateEnum.h"
#include "Alpha/Objects/Boss/Proto/BossGameObject.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Navmesh/NavmeshSubsystem.h"

BossState_Phase1_Travel::BossState_Phase1_Travel(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
	, m_targetPos()
{ }

BossState_Phase1_Travel::~BossState_Phase1_Travel()
{ }

void BossState_Phase1_Travel::OnEnter()
{
	Systems::NavmeshSubsystem* pNavmesh = Systems::NavmeshSubsystem::GetSubsystem();
	m_targetPos = pNavmesh->QueryRandomPosition();

	//Core::LogModule::Get().LogInfo("BossState_Phase1_Travel::OnEnter");
}

void BossState_Phase1_Travel::OnUpdate()
{
	Systems::GameMgr& gameMgr = Systems::GameMgr().Get();
	Systems::GameContext* pContext = gameMgr.GetWorld();

	const float SPEED = 20;
	float dt = pContext->m_pClock->GetDeltaTime();

	Core::Vec4f oldPosition = m_pBoss->GetTransform().GetWorldTx().GetT();
	Core::Vec4f dir = m_targetPos - oldPosition;

	Core::Vec4f newPosition = oldPosition;

	float distance = dir.Length();
	float minDistance = SPEED * dt;
	if (distance <= minDistance)
	{
		newPosition = m_targetPos;
		GoTo(BossStateEnum::PHASE1_ATTACK);
	}
	else
	{
		dir = dir * (1.f / distance); //normalize without recomputing the distance
		float displacement = SPEED * dt;
		newPosition = oldPosition + dir * displacement;
		//Core::LogModule::Get().LogInfo("dt : %f", dt);
	}

	Core::Mat44f wsTx = Core::Mat44f::CreateTranslationMatrix(newPosition);
	m_pBoss->GetTransform().SetLocalTx(wsTx);
}

void BossState_Phase1_Travel::OnExit()
{ }
