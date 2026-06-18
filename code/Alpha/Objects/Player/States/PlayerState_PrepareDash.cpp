/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_PrepareDash.h"

#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"
#include "Alpha/Objects/Player/States/PlayerStateContext.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"

#include "Core/Math/Vec4f.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

PlayerState_PrepareDash::PlayerState_PrepareDash(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
	, m_elapsedTime(0)
{ }

void PlayerState_PrepareDash::OnEnter()
{
	m_elapsedTime = 0;
	Systems::GameMgr::GetClock()->SetTimeScale(0.1f);

	m_pPlayer->ShowDashCircle();
}

void PlayerState_PrepareDash::OnUpdate()
{
	const Core::Vec4f FORWARD(0, 0, 1, 0);
	const Core::Vec4f RIGHT(1, 0, 0, 0);

	Core::Vec4f direction(0, 0, 0, 1);
	if (GameCommands::MoveUp())
		direction = direction + FORWARD;
	else if (GameCommands::MoveDown())
		direction = direction - FORWARD;

	if (GameCommands::MoveLeft())
		direction = direction - RIGHT;
	else if (GameCommands::MoveRight())
		direction = direction + RIGHT;

	direction.Normalize();

	if (direction.Length2() > 0)
	{
		m_pPlayer->ShowDashTarget();

		const float DASH_DISTANCE = 10;
		direction = direction * DASH_DISTANCE;
		direction.Set(3, 1);
		m_pPlayer->SetDashTargetRelativePosition(direction);
	}
	else
	{
		m_pPlayer->HideDashTarget();
	}

	if (GameCommands::Dash() == 0)
	{
		if (direction.Length2() == 0)
		{
			m_pPlayer->HideDashTarget();
			GoTo(PlayerStateEnum::MOVE);
		}
		else
		{
			GetContext<PlayerStateContext>()->m_dashTargetRelativePosition = direction;
			GoTo(PlayerStateEnum::DASH); //should go to dash
		}
	}
}

void PlayerState_PrepareDash::OnExit()
{
	Systems::GameMgr::GetClock()->SetTimeScale(1);
	m_pPlayer->HideDashCircle();
}
