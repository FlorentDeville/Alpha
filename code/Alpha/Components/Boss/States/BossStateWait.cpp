/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossStateWait.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Navmesh/NavmeshSubsystem.h"

BossStateWait::BossStateWait(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
	, m_targetPos()
{ }

BossStateWait::~BossStateWait()
{ }

void BossStateWait::OnEnter()
{
	Systems::NavmeshSubsystem* pNavmesh = Systems::NavmeshSubsystem::GetSubsystem();
	m_targetPos = pNavmesh->QueryRandomPosition();

	//Core::LogModule::Get().LogInfo("BossStateWait::OnEnter");
}

void BossStateWait::OnUpdate()
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
		GoTo(BossStateEnum::WAVE_TEST);
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

void BossStateWait::OnExit()
{ }
