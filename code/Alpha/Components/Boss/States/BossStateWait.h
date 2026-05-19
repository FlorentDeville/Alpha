/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

#include "Core/Math/Vec4f.h"

class BossGameObject;

class BossStateWait : public IState
{
public:
	BossStateWait(StateMachine* pStateMachine, BossGameObject* pBoss);
	~BossStateWait();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	//float m_startTime;
	//float m_waitTime;
	BossGameObject* m_pBoss;

	Core::Vec4f m_targetPos;
};
