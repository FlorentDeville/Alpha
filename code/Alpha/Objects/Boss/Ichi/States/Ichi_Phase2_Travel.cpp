/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Travel.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"

#include "Systems/Game/GameMgr.h"

Ichi_Phase2_Travel::Ichi_Phase2_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_currentWaypoint(0)
	, m_nextAttack(false)
{
	//these should not be hardcoded.
	m_waypoint[0] = Core::Vec4f(-25, 0, 15, 1);
	m_waypoint[1] = Core::Vec4f(25, 0, 15, 1);
}

Ichi_Phase2_Travel::~Ichi_Phase2_Travel()
{ }

void Ichi_Phase2_Travel::OnEnter()
{
	PlayerGameObject* pPlayer = Systems::GameMgr::Get().FindGameObject<PlayerGameObject>();

	Core::Vec4f playerForward(1, 0, 0, 0);
	Core::Vec4f bossToPlayer = pPlayer->GetTransform().GetWorldPosition() - m_pIchi->GetTransform().GetWorldPosition();
	bossToPlayer.Set(2, 0);

	float direction = playerForward.Dot(bossToPlayer);

	if (direction < 0)
		m_currentWaypoint = 0;
	else
		m_currentWaypoint = 1;

	const float MAX_SPEED = 7;
	m_pIchi->GoToMotionStateTravel(m_waypoint[m_currentWaypoint], MAX_SPEED);
}

void Ichi_Phase2_Travel::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP) || m_pIchi->IsInMotionState(IchiMotionState::IDLE))
	{
		if (!m_nextAttack)
			GoTo(IchiStateEnum::PHASE2_ATTACK1);
		else
			GoTo(IchiStateEnum::PHASE2_ATTACK2);

		m_currentWaypoint = (m_currentWaypoint + 1) % WAYPOINT_COUNT;
	}
}

void Ichi_Phase2_Travel::OnExit()
{
	m_nextAttack = !m_nextAttack;
}
