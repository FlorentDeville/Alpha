/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/StateMachine/IState.h"
#include "Alpha/Objects/Boss/Ichi/Ichi.h"

class Ichi;

class Ichi_Dying : public IState
{
public:
	Ichi_Dying(StateMachine* pStateMachine, Ichi* pIchi);
	~Ichi_Dying();

	void OnEnter() override;
	void OnUpdate() override;
	void OnExit() override;

private:
	enum InternalState
	{
		PREPARE,
		START,
		PAUSE,
		LOWER_TOWER,
		MIDDLE_TOWER,
		UPPER_TOWER,
		STOP,
		OVER,

		COUNT
	};

	Ichi* m_pIchi;

	InternalState m_internalState;
	float m_internlStateStartTime;

	float m_rumbleStartTime;
	Core::Vec4f m_rumbleInitialPosition;

	void GoToInternalState(InternalState nextState);

	void Rumble();

	void Fall(Systems::RenderableComponent* pMesh, float fallingSpeed, float rotationSpeed) const;

	bool IsElaspedTime(float maxDuration) const;
};
