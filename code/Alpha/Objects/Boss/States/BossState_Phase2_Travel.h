/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Bezier/Bezier.h"
#include "Core/Math/Vec4f.h"

class BossGameObject;

class BossState_Phase2_Travel : public IState
{
public:
	BossState_Phase2_Travel(StateMachine* pStateMachine, BossGameObject* pBoss);
	~BossState_Phase2_Travel();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	BossGameObject* m_pBoss;

	Core::QuadraticBezier m_curve;
	float m_curveParam;

	bool m_nextAttackIsAttack1;
};
