/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase3_Travel.h"

#include "Alpha/Objects/Boss/Ichi/States/IchiStateEnum.h"

Ichi_Phase3_Travel::Ichi_Phase3_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
	, m_currentTarget(0)
	, m_nextAttack(false)
{
}

Ichi_Phase3_Travel::~Ichi_Phase3_Travel()
{
}

void Ichi_Phase3_Travel::OnEnter()
{
	m_currentTarget = 0;

	//compute final point
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	m_target[0] = currentPosition + Core::Vec4f(20, 0, 0, 0);
	m_target[1] = currentPosition;

	if (m_pIchi->GetCurrentHP() <= 0)
	{
		GoTo(IchiStateEnum::DYING);
	}
}

void Ichi_Phase3_Travel::OnUpdate()
{
	if (m_pIchi->IsInMotionState(IchiMotionState::STOP) || m_pIchi->IsInMotionState(IchiMotionState::IDLE))
	{
		if (m_currentTarget >= TARGET_COUNT)
		{
			if (!m_nextAttack)
				GoTo(IchiStateEnum::PHASE3_ATTACK1);
			else
				GoTo(IchiStateEnum::PHASE3_ATTACK2);

			return;
		}

		m_pIchi->GoToMotionStateTravel(m_target[m_currentTarget]);
		++m_currentTarget;
	}
}

void Ichi_Phase3_Travel::OnExit()
{
	m_nextAttack = !m_nextAttack;
}
