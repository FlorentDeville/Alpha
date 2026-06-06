/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Boss/Ichi/States/Ichi_Phase1_Travel.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

Ichi_Phase1_Travel::Ichi_Phase1_Travel(StateMachine* pStateMachine, Ichi* pIchi)
	: IState(pStateMachine)
	, m_pIchi(pIchi)
{ }

void Ichi_Phase1_Travel::OnEnter()
{
	m_currentTarget = 0;

	//compute final point
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	m_target[0] = currentPosition + Core::Vec4f(20, 0, 0, 0);
	m_target[1] = currentPosition;// -Core::Vec4f(20, 0, 0, 0);
}

void Ichi_Phase1_Travel::OnUpdate()
{
	if (m_currentTarget >= TARGET_COUNT)
		return;

	if (!m_pIchi->IsInMotionState(IchiMotionState::STOP))
		return;

	m_pIchi->GoToMotionStateTravel(m_target[m_currentTarget]);

	++m_currentTarget;
}

void Ichi_Phase1_Travel::OnExit()
{ }
