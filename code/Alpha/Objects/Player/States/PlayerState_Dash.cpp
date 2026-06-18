/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/Player/States/PlayerState_Dash.h"

#include "Alpha/Objects/Player/PlayerGameObject.h"
#include "Alpha/Objects/Player/States/PlayerStateContext.h"
#include "Alpha/Objects/Player/States/PlayerStateEnum.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Clock/IClockSubsystem.h"

PlayerState_Dash::PlayerState_Dash(StateMachine* pMachine, PlayerGameObject* pPlayer)
	: IState(pMachine)
	, m_pPlayer(pPlayer)
{ }

void PlayerState_Dash::OnEnter()
{
	m_curve.m_p0 = m_pPlayer->GetTransform().GetLocalSqt().GetTranslation();
	m_curve.m_p2 = m_curve.m_p0 + GetContext< PlayerStateContext>()->m_dashTargetRelativePosition;
	m_curve.m_p2.Set(3, 1);

	Core::Vec4f halfWay = (m_curve.m_p2 - m_curve.m_p0) * 0.5f;
	Core::Vec4f orthoHalfWay(halfWay.GetZ(), halfWay.GetY(), -halfWay.GetX(), 0);
	m_curve.m_p1 = m_curve.m_p0 + halfWay + orthoHalfWay;

	m_startTime = Systems::GameMgr::GetClock()->GetTime();
}

void PlayerState_Dash::OnUpdate()
{ 
	const float DURATION = 0.4f;
	float currentTime = Systems::GameMgr::GetClock()->GetTime();

	float t = (currentTime - m_startTime) / DURATION;

	if (t >= 1)
	{
		m_pPlayer->GetTransform().SetLocalTranslation(m_curve.m_p2);
		GoTo(PlayerStateEnum::MOVE);
		return;
	}

	Core::Vec4f newPosition = m_curve.Evaluate(t);
	m_pPlayer->GetTransform().SetLocalTranslation(newPosition);

	const Core::Vec4f& targetRelPos = GetContext< PlayerStateContext>()->m_dashTargetRelativePosition;
	m_pPlayer->SetDashTargetRelativePosition(targetRelPos + (m_curve.m_p0 - newPosition));
}

void PlayerState_Dash::OnExit()
{
	m_pPlayer->HideDashTarget();
}
