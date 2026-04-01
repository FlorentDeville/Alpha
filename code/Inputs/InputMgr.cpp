/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "InputMgr.h"

#include "OsWin/VirtualKeyCode.h"

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
	{ }

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
		Os::VKeyCode vkey;

		switch (command)
		{
		case InputCommand::FORWARD:
			vkey = Os::VKeyCodes::Vk_W;
			break;

		case InputCommand::BACKWARD:
			vkey = Os::VKeyCodes::Vk_S;
			break;

		case InputCommand::LEFT:
			vkey = Os::VKeyCodes::Vk_A;
			break;

		case InputCommand::RIGHT:
			vkey = Os::VKeyCodes::Vk_D;
			break;
		}

		return IsKeyPressed(vkey);
	}

	void InputMgr::ClearAllStates()
	{
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

	const Core::Int2& InputMgr::GetMousePosition() const
	{
		return m_mouseState.m_mousePos;
	}

	int16_t InputMgr::GetMouseWheelDistance() const
	{
		return m_mouseState.m_mouseWheel;
	}
}
