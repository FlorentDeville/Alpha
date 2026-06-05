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
	//compute final point
	Core::Vec4f currentPosition = m_pIchi->GetTransform().GetWorldTx().GetT();
	Core::Vec4f target = currentPosition + Core::Vec4f(20, 0, 0, 0);

	m_pIchi->GoToMotionStateTravel(target);
}

void Ichi_Phase1_Travel::OnUpdate()
{ 
}

void Ichi_Phase1_Travel::OnExit()
{ }
