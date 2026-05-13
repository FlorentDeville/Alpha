/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/Message/GameMessageSubsystem.h"

#include "Systems/Game/Subsystems/Message/GameMessage.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Objects/GameObject.h"

namespace Systems
{
	uint32_t GameMessageSubsystem::m_subsystemIndex = UINT32_MAX;

	GameMessageSubsystem::GameMessageSubsystem()
		: ISubsystem()
		, m_messagesList()
		, m_currentListIndex(0)
	{
		m_messagesList[0].Reserve(100);
		m_messagesList[1].Reserve(100);
	}

	GameMessageSubsystem::~GameMessageSubsystem()
	{ }

	void GameMessageSubsystem::BeforeUpdate(const GameContext& context)
	{
		//swap the list.
		uint32_t listToExecute = m_currentListIndex;
		m_currentListIndex = (m_currentListIndex + 1) % 1;

		for (const GameMessageContainer& container : m_messagesList[listToExecute])
		{
			container.m_pGo->HandleMessage(container.m_message);
		}
	}

	void GameMessageSubsystem::SendMessage(GameObject* pGo, GameMessage& msg)
	{
		GameMessageContainer& container = m_messagesList[m_currentListIndex].PushBackDefault();
		container.m_pGo = pGo;
		container.m_message = msg;
	}

	GameMessageSubsystem* GameMessageSubsystem::GetSubsystem()
	{
		ISubsystem* pSubsystem = GameMgr::Get().GetGameSubsystem(m_subsystemIndex);
		return static_cast<GameMessageSubsystem*>(pSubsystem);
	}

}
