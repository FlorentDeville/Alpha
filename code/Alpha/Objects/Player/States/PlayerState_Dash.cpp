/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_Dash.h"

#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"

#include "Core/Math/Vec4f.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

PlayerState_Dash::PlayerState_Dash(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
	, m_elapsedTime(0)
{ }

void PlayerState_Dash::OnEnter()
{
	m_elapsedTime = 0;
	Systems::GameMgr::GetClock()->SetTimeScale(0.1f);
}

void PlayerState_Dash::OnUpdate()
{
	if(GameCommands::Dash() == 0)
		GoTo(PlayerStateEnum::MOVE);

	/*float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();
	m_elapsedTime += dt;
	const float DASH_DURATION = 0.2f;
	if (m_elapsedTime >= DASH_DURATION)
	{
		GoTo(PlayerStateEnum::MOVE);
		return;
	}
	
	Systems::TransformComponent& transform = m_pPlayer->GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	const Core::Vec4f forward = localTx.GetZ();
	const Core::Vec4f right = localTx.GetX();
	const Core::Vec4f oldPosition = localTx.GetT();

	Core::Vec4f direction;
	if (GameCommands::MoveUp())
		direction = direction + forward;
	else if (GameCommands::MoveDown())
		direction = direction - forward;

	if (GameCommands::MoveLeft())
		direction = direction - right;
	else if (GameCommands::MoveRight())
		direction = direction + right;

	const float DASH_SPEED = 50;
	direction.Normalize();
	Core::Vec4f newPosition = oldPosition + direction * DASH_SPEED * dt;

	Core::Mat44f newLocalTx = localTx;
	newLocalTx.SetRow(3, newPosition);

	transform.SetLocalTx(newLocalTx);	*/
}

void PlayerState_Dash::OnExit()
{
	Systems::GameMgr::GetClock()->SetTimeScale(1);
}
