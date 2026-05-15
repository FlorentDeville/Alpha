/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Inputs/InputCommand.h"

#include "Inputs/InputMgr.h"

namespace Inputs
{
	InputKeyCommand::InputKeyCommand(Os::VKeyCode key, Behavior behavior)
		: m_key(key)
		, m_behavior(behavior)
		, m_state(0)
		, m_wasUp(true)
	{ }

	void InputKeyCommand::Update()
	{
		const InputMgr& inputMgr = InputMgr::Get();

		switch (m_behavior)
		{
		case Behavior::DOWN:
			m_state = inputMgr.IsKeyPressed(m_key) ? 1.f : 0.f;
			break;

		case Behavior::UP:
			m_state = inputMgr.IsKeyPressed(m_key) ? 0.f : 1.f;
			break;

		case Behavior::PRESSED:
		{
			if (m_state == 0)
			{
				if (m_wasUp)
					m_state = inputMgr.IsKeyPressed(m_key) ? 1.f : 0.f;
				else
					m_wasUp = !inputMgr.IsKeyPressed(m_key);
			}
			else
			{
				m_state = 0.f;
				m_wasUp = false;
			}
		}
		break;

		}
	}
}
