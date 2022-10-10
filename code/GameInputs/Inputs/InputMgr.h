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
		struct MouseState
		{
			bool m_mouseLeftButton;
			bool m_mouseMiddleButton;
			bool m_mouseRightButton;
			float m_mouseWheel;
			uint32_t m_mouseX;
			uint32_t m_mouseY;

			MouseState()
				: m_mouseLeftButton(false)
				, m_mouseMiddleButton(false)
				, m_mouseRightButton(false)
				, m_mouseX(0)
				, m_mouseY(0)
				, m_mouseWheel(0)
			{}
		};

		InputMgr();
		virtual ~InputMgr();

		void Init();
		void Release();

		void UpdateKeyboard(uint64_t virtualKey);
		void UpdateMouseState(const MouseState& mouseState);

		bool GetState(InputCommand command) const;

		void ClearAllStates();

		void Enable();
		void Disable();

		//use only in editor, not in game, game should use commands
		bool IsKeyPressed(char key) const;
		bool IsMouseLeftButtonDown() const;
		void GetMousePosition(uint32_t& x, uint32_t& y) const;
		int16_t GetMouseWheelDistance() const;

	private:
		//game command
		std::map<uint64_t, InputCommand> m_keyToCommand;
		std::map<InputCommand, bool> m_commandState;

		bool m_enabled;

		//raw keyboard mouse event
		std::map<uint64_t, bool> m_keyboardState; //true for down
		bool m_mouseLeftButton;
		MouseState m_mouseState;
	};
}
