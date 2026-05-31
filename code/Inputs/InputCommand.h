/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "OsWin/VirtualKeyCode.h"

namespace Inputs
{
	class InputKeyCommand
	{
	public:
		enum Behavior : uint8_t
		{
			DOWN,		// triggered every frame the key is down.
			PRESSED,	// triggered once when the key goes down. The key needs to be released before PRESSED can be fired again.
			UP,			// triggered every frame when the key is up.

			COUNT
		};

		InputKeyCommand() = default;
		InputKeyCommand(Os::VKeyCode key, Behavior behavior);
		~InputKeyCommand() = default;

		void Update();

		float GetState() const;

	private:
		Os::VKeyCode m_key;
		Behavior m_behavior;

		bool m_wasUp;

		float m_state;
	};
}
