/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_Move.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Inputs/GameCommands.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"
#include "Alpha/Objects/Player/States/PlayerStateContext.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

PlayerState_Move::PlayerState_Move(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
{ }

void PlayerState_Move::OnEnter()
{ }

void PlayerState_Move::OnUpdate()
{
	float dt = Systems::GameMgr::Get().GetWorld()->m_pClock->GetDeltaTime();

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

	if (direction.Length2() != 0)
	{
		direction.Normalize();
		Core::Vec4f newPosition = oldPosition + direction * m_pPlayer->GetSpeed() * dt;

		Core::Mat44f newLocalTx = localTx;
		newLocalTx.SetRow(3, newPosition);

		transform.SetLocalTx(newLocalTx);
	}

	PlayerStateContext* pContext = GetContext<PlayerStateContext>();

	if (GameCommands::Counter())
	{
		if (pContext->m_counterableBulletIndex != UINT32_MAX)
		{
			BulletSubsystem* bulletSubsystem = BulletSubsystem::GetSubsystem();

			const Core::Vec4f position = bulletSubsystem->GetBulletPosition(pContext->m_counterableBulletIndex);
			m_pPlayer->SpawnCounteredBullet(position);

			bulletSubsystem->KillBullet(pContext->m_counterableBulletIndex);
		}
	}

	if (GameCommands::Dash())
	{
		GoTo(PlayerStateEnum::PREPARE_DASH);		
	}

	pContext->m_counterableBulletIndex = UINT32_MAX;
}

void PlayerState_Move::OnExit()
{ }
