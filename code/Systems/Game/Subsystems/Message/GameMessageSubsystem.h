/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Systems/Game/Subsystems/ISubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"

namespace Systems
{
	class GameObject;

	class GameMessageSubsystem : public ISubsystem
	{
	public:
		GameMessageSubsystem();
		~GameMessageSubsystem();

		void BeforeUpdate(const GameContext& context) override;

		//This message will not be execute right away. Messages are all handled at the beginning of the frame, before Update.
		void SendMessage(GameObject* pGo, GameMessage& msg);

		static uint32_t m_subsystemIndex;
		static GameMessageSubsystem* GetSubsystem();

	private:
		class GameMessageContainer
		{
		public:
			GameObject* m_pGo;
			GameMessage m_message;
		};
		
		uint32_t m_currentListIndex;
		Core::Array<GameMessageContainer> m_messagesList[2];
	};
}
