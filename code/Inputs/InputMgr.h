/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vectors.h"
#include "Core/Singleton.h"

#include "InputCommand.h"

#include <map>

namespace Inputs
{
	class InputMgr : public Core::Singleton<InputMgr>
	{
	public:
		struct MouseState
		{
			bool m_mouseLeftButton;
			bool m_mouseMiddleButton;
			bool m_mouseRightButton;
			int16_t m_mouseWheel;
			Core::Int2 m_mousePos; //The mouse position can be negative if outside of the client area.

			MouseState()
				: m_mouseLeftButton(false)
				, m_mouseMiddleButton(false)
				, m_mouseRightButton(false)
				, m_mousePos(0, 0)
				, m_mouseWheel(0)
			{}
		};

		InputMgr();
		virtual ~InputMgr();

		void Init();
		void Release();

		void UpdateKeyboardState(uint64_t virtualKey, bool down);
		void UpdateMouseState(const MouseState& mouseState);

		bool GetState(InputCommand command) const;

		void ClearAllStates();

		void Enable();
		void Disable();

		//use only in editor, not in game, game should use commands
		const MouseState& GetMouseState() const;
		bool IsKeyPressed(char key) const;
		bool IsMouseLeftButtonDown() const;
		bool IsMouseMiddleButtonDown() const;
		bool IsMouseRightButtonDown() const;
		const Core::Int2& GetMousePosition() const;
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
