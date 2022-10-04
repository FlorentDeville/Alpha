/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "InputCommand.h"

#include <map>

namespace GameInputs
{
	class InputMgr : public Core::Singleton<InputMgr>
	{
	public:
		InputMgr();
		virtual ~InputMgr();

		void Init();
		void Release();

		void Update(uint64_t virtualKey);

		bool GetState(InputCommand command) const;

		void ClearAllStates();

		void Enable();
		void Disable();

		//use only in editor, not in game, game should use commands
		bool IsKeyPressed(char key) const;

	private:
		//game command
		std::map<uint64_t, InputCommand> m_keyToCommand;
		std::map<InputCommand, bool> m_commandState;

		bool m_enabled;

		//raw keyboard mouse event
		std::map<uint64_t, bool> m_keyboardState; //true to down
	};
}
