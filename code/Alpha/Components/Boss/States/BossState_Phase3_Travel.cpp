/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/States/BossState_Phase3_Travel.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Core/Log/LogModule.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"
#include "Systems/Game/Subsystems/Navmesh/NavmeshSubsystem.h"

BossState_Phase3_Travel::BossState_Phase3_Travel(StateMachine* pStateMachine, BossGameObject* pBoss)
	: IState(pStateMachine)
	, m_pBoss(pBoss)
	, m_curve()
	, m_curveParam(0)
	, m_nextAttackIsAttack1(true)
{
}

BossState_Phase3_Travel::~BossState_Phase3_Travel()
{
}

void BossState_Phase3_Travel::OnEnter()
{
	Systems::NavmeshSubsystem* pNavmesh = Systems::NavmeshSubsystem::GetSubsystem();
	m_curve.m_p0 = m_pBoss->GetTransform().GetWorldTx().GetT();
	m_curve.m_p1 = pNavmesh->QueryRandomPosition();
	m_curve.m_p2 = pNavmesh->QueryRandomPosition();

	m_curveParam = 0;
	//Core::LogModule::Get().LogInfo("BossState_Phase2_Travel::OnEnter");
}

void BossState_Phase3_Travel::OnUpdate()
{
	if (m_curveParam >= 1)
	{
		if (m_nextAttackIsAttack1)
			GoTo(BossStateEnum::PHASE3_ATTACK1);
		else
			GoTo(BossStateEnum::PHASE3_ATTACK2);

		m_nextAttackIsAttack1 = !m_nextAttackIsAttack1;
		return;
	}

	Systems::GameMgr& gameMgr = Systems::GameMgr().Get();
	Systems::GameContext* pContext = gameMgr.GetWorld();

	const float SPEED = 35;
	float ds = SPEED * pContext->m_pClock->GetDeltaTime();

	m_curveParam += ds / m_curve.EvaluateFirstDerivative(m_curveParam).Length();
	if (m_curveParam >= 1)
		m_curveParam = 1;

	Core::Vec4f newPosition = m_curve.Evaluate(m_curveParam);

	Core::Mat44f wsTx = Core::Mat44f::CreateTranslationMatrix(newPosition);
	m_pBoss->GetTransform().SetLocalTx(wsTx);
}

void BossState_Phase3_Travel::OnExit()
{
}
