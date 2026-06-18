/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"

class PlayerGameObject;

class PlayerState_Move : public IState
{
public:
	PlayerState_Move(StateMachine* pMachine, PlayerGameObject* pPlayer);
	~PlayerState_Move() = default;

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	PlayerGameObject* m_pPlayer;
};
