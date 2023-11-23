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

	void InputMgr::UpdateKeyboardState(uint64_t virtualKey, bool down)
	{
		m_keyboardState[virtualKey] = down;
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

		m_mouseState.m_mouseWheel = 0; //reset the mouse wheel distance
	}

	void InputMgr::Enable()
	{
		m_enabled = true;
	}

	void InputMgr::Disable()
	{
		m_enabled = false;
	}

	const InputMgr::MouseState& InputMgr::GetMouseState() const
	{
		return m_mouseState;
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

	bool InputMgr::IsMouseMiddleButtonDown() const
	{
		return m_mouseState.m_mouseMiddleButton;
	}

	bool InputMgr::IsMouseRightButtonDown() const
	{
		return m_mouseState.m_mouseRightButton;
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
