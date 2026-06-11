/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Systems/Game/GameMgr.h"

Ichi_Phase1_Travel::Ichi_Phase1_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_currentTarget(0)
	, m_nextAttack(false)
{
	//these should not be hardcoded.
	m_target[0] = Core::Vec4f(-25, 0, 15, 1);
	m_target[1] = Core::Vec4f(25, 0, 15, 1);
}

void Ichi_Phase1_Travel::OnEnter()
{
	PlayerGameObject* pPlayer = Systems::GameMgr::Get().FindGameObject<PlayerGameObject>();

	Core::Vec4f playerForward(1, 0, 0, 0);
	Core::Vec4f bossToPlayer = pPlayer->GetTransform().GetWorldPosition() - m_pIchi->GetTransform().GetWorldPosition();
	bossToPlayer.Set(2, 0);

	float direction = playerForward.Dot(bossToPlayer);

	if (direction < 0)
		m_currentTarget = 0;
	else
		m_currentTarget = 1;

	const float MAX_SPEED = 5;
	m_pIchi->GoToMotionStateTravel(m_target[m_currentTarget], MAX_SPEED);
}

void Ichi_Phase1_Travel::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP))
	{
		if (!m_nextAttack)
			GoTo(IchiStateEnum::PHASE1_ATTACK1);
		else
			GoTo(IchiStateEnum::PHASE1_ATTACK2);

		m_currentTarget = (m_currentTarget + 1) % TARGET_COUNT;
	}
}

void Ichi_Phase1_Travel::OnExit()
{
	m_nextAttack = !m_nextAttack;
}
