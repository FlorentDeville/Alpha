/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "InputMgr.h"

//#pragma optimize("", off)

namespace Inputs
{
	InputMgr::InputMgr()
		: m_enabled(false)
		, m_mouseState()
	{}

	InputMgr::~InputMgr()
	{}

	void InputMgr::Init()
	{
		m_keyToCommand['w'] = InputCommand::MoveForward;
		m_keyToCommand['W'] = InputCommand::MoveForward;
		m_keyToCommand['s'] = InputCommand::MoveBackward;
		m_keyToCommand['S'] = InputCommand::MoveBackward;
		m_keyToCommand['a'] = InputCommand::MoveLeft;
		m_keyToCommand['A'] = InputCommand::MoveLeft;
		m_keyToCommand['d'] = InputCommand::MoveRight;
		m_keyToCommand['D'] = InputCommand::MoveRight;
	}

	void InputMgr::Release()
	{}

	void InputMgr::UpdateKeyboard(uint64_t virtualKey)
	{
		m_keyboardState[virtualKey] = true;

		if (!m_enabled)
			return;

		std::map<uint64_t, InputCommand>::const_iterator it = m_keyToCommand.find(virtualKey);
		if (it == m_keyToCommand.cend())
			return;

		m_commandState[it->second] = true;
	}

	void InputMgr::UpdateMouseState(const MouseState& mouseState)
	{
		m_mouseState = mouseState;
	}

	bool InputMgr::GetState(InputCommand command) const
	{
		std::map<InputCommand, bool>::const_iterator it = m_commandState.find(command);
		if (it == m_commandState.cend())
			return false;

		return it->second;
	}

	void InputMgr::ClearAllStates()
	{
		for (std::map<InputCommand, bool>::iterator it = m_commandState.begin(); it != m_commandState.end(); ++it)
			it->second = false;

		for (std::map<uint64_t, bool>::iterator it = m_keyboardState.begin(); it != m_keyboardState.end(); ++it)
			it->second = false;

		m_mouseState = MouseState();
	}

	void InputMgr::Enable()
	{
		m_enabled = true;
	}

	void InputMgr::Disable()
	{
		m_enabled = false;
	}

	bool InputMgr::IsKeyPressed(char key) const
	{
		std::map<uint64_t, bool>::const_iterator it = m_keyboardState.find(key);
		if (it == m_keyboardState.cend())
			return false;

		return it->second;
	}

	bool InputMgr::IsMouseLeftButtonDown() const
	{
		return m_mouseState.m_mouseLeftButton;
	}

	void InputMgr::GetMousePosition(uint32_t& x, uint32_t& y) const
	{
		x = m_mouseState.m_mouseX;
		y = m_mouseState.m_mouseY;
	}

	int16_t InputMgr::GetMouseWheelDistance() const
	{
		return m_mouseState.m_mouseWheel;
	}
}
