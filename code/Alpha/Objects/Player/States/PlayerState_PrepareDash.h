/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class PlayerGameObject;

class PlayerState_PrepareDash : public IState
{
public:
	PlayerState_PrepareDash(StateMachine* pMachine, PlayerGameObject* pPlayer);
	~PlayerState_PrepareDash() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	PlayerGameObject* m_pPlayer;

	float m_elapsedTime;
};
