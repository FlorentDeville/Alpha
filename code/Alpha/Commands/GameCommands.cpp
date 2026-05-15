/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Commands/GameCommands.h"

#include "Inputs/InputMgr.h"

#include <cstdint>

namespace GameCommands
{
	enum GameCommandId : uint8_t
	{
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,

		COUNTER,

		COUNT
	};

	static uint32_t s_commandIndex[GameCommandId::COUNT];

	float GetState(GameCommandId index)
	{
		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();
		return inputMgr.GetState(s_commandIndex[index]);
	}

	void RegisterGameCommands()
	{
		Inputs::InputKeyCommand commandUp(Os::VKeyCodes::Vk_W, Inputs::InputKeyCommand::DOWN);
		Inputs::InputKeyCommand commandDown(Os::VKeyCodes::Vk_S, Inputs::InputKeyCommand::DOWN);
		Inputs::InputKeyCommand commandLeft(Os::VKeyCodes::Vk_A, Inputs::InputKeyCommand::DOWN);
		Inputs::InputKeyCommand commandRight(Os::VKeyCodes::Vk_D, Inputs::InputKeyCommand::DOWN);
		Inputs::InputKeyCommand commandCounter(Os::VKeyCodes::Return, Inputs::InputKeyCommand::PRESSED);

		Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();

		s_commandIndex[GameCommandId::MOVE_UP] = inputMgr.RegisterCommand(commandUp);
		s_commandIndex[GameCommandId::MOVE_DOWN] = inputMgr.RegisterCommand(commandDown);
		s_commandIndex[GameCommandId::MOVE_LEFT] = inputMgr.RegisterCommand(commandLeft);
		s_commandIndex[GameCommandId::MOVE_RIGHT] = inputMgr.RegisterCommand(commandRight);
		s_commandIndex[GameCommandId::COUNTER] = inputMgr.RegisterCommand(commandCounter);
	}

	float MoveLeft()
	{
		return GetState(GameCommandId::MOVE_LEFT);
	}

	float MoveRight()
	{
		return GetState(GameCommandId::MOVE_RIGHT);
	}

	float MoveUp()
	{
		return GetState(GameCommandId::MOVE_UP);
	}

	float MoveDown()
	{
		return GetState(GameCommandId::MOVE_DOWN);
	}

	float Counter()
	{
		return GetState(GameCommandId::COUNTER);
	}
}
