/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase2_Travel.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

Ichi_Phase2_Travel::Ichi_Phase2_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_currentTarget(0)
	, m_nextAttack(false)
{
}

Ichi_Phase2_Travel::~Ichi_Phase2_Travel()
{ }

void Ichi_Phase2_Travel::OnEnter()
{
	m_currentTarget = 0;

	//compute final point
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	m_target[0] = currentPosition + Core::Vec4f(20, 0, 0, 0);
	m_target[1] = currentPosition;

	if (m_pIchi->GetCurrentHP() <= 0)
	{
		GoTo(IchiStateEnum::PHASE2_TO_PHASE3);
	}
}

void Ichi_Phase2_Travel::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP) || m_pIchi->IsInMotionState(IchiMotionState::IDLE))
	{
		if (m_currentTarget >= TARGET_COUNT)
		{
			if (!m_nextAttack)
				GoTo(IchiStateEnum::PHASE2_ATTACK1);
			else
				GoTo(IchiStateEnum::PHASE2_ATTACK2);

			return;
		}

		m_pIchi->GoToMotionStateTravel(m_target[m_currentTarget]);
		++m_currentTarget;
	}
}

void Ichi_Phase2_Travel::OnExit()
{
	m_nextAttack = !m_nextAttack;
}
