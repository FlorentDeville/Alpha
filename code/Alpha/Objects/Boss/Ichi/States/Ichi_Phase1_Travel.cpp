/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

Ichi_Phase1_Travel::Ichi_Phase1_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_currentTarget(0)
	, m_nextAttack(false)
	, m_targetInitialized(false)
{ }

void Ichi_Phase1_Travel::OnEnter()
{
	//compute final point
	if (!m_targetInitialized)
	{
		Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
		m_target[0] = currentPosition + Core::Vec4f(20, 0, 0, 0);
		m_target[1] = currentPosition;

		m_targetInitialized = true;
	}

	m_pIchi->GoToMotionStateTravel(m_target[m_currentTarget]);

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
