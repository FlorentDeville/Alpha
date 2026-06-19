/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_Counter.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Objects/Player/PlayerGameObject.h"
#include "Alpha/Objects/Player/States/PlayerStateContext.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"

#include "Core/Math/Constants.h"

#include "Systems/GameComponent/RenderableComponent.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

PlayerState_Counter::PlayerState_Counter(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
	, m_yAngle(0)
	, m_startRotation()
	, m_direction(1)
{ 
	m_pRenderable = m_pPlayer->FindComponent<Systems::RenderableComponent>();
}

void PlayerState_Counter::OnEnter()
{
	const PlayerStateContext* pContext = GetContext<PlayerStateContext>();

	if (pContext->m_counterableBulletIndex != UINT32_MAX)
	{
		BulletSubsystem* bulletSubsystem = BulletSubsystem::GetSubsystem();

		const Core::Vec4f position = bulletSubsystem->GetBulletPosition(pContext->m_counterableBulletIndex);
		m_pPlayer->SpawnCounteredBullet(position);

		bulletSubsystem->KillBullet(pContext->m_counterableBulletIndex);
	}

	m_yAngle = 0;
	m_startRotation = m_pRenderable->GetLocalTx().GetRotationQuaternion();
	m_direction = -m_direction;
}

void PlayerState_Counter::OnUpdate()
{ 
	const float ROTATION_SPEED = 32;
	float dt = Systems::GameMgr::GetClock()->GetDeltaTime();

	float localAngle = ROTATION_SPEED * dt * m_direction;
	m_yAngle += localAngle;

	if (abs(m_yAngle) >= Core::TWO_PI)
	{
		m_pRenderable->SetLocalRotation(m_startRotation);
		GoTo(PlayerStateEnum::MOVE);
		return;
	}

	Core::Quaternion rotation = Core::Quaternion::FromEulerAngles(0, localAngle, 0);
	const Core::Quaternion& currentRot = m_pRenderable->GetLocalTx().GetRotationQuaternion();

	Core::Quaternion newRotation = rotation * currentRot;
	m_pRenderable->SetLocalRotation(newRotation);
}

void PlayerState_Counter::OnExit()
{ }
