/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "OsWin/VirtualKeyCode.h"

#include "Core/Callbacks/CallbackList.h"

namespace Widgets
{
	class Widget;

	class Shortcut
	{
	public:
		Shortcut(const std::string& keySequence, Widget* pParent);
		~Shortcut();

		bool Evaluate();

		using OnActivatedEvent = Core::CallbackList<void()>;

		Core::CallbackId OnActivated(const OnActivatedEvent::Callback& callback);

	private:
		static const int MAX_KEY_COUNT = 3;
		Os::VKeyCode m_vkeyCodes[MAX_KEY_COUNT];
		uint8_t m_keyCount;

		OnActivatedEvent m_onActivated;

		Widget* m_pParentWidget;

		void ParseKeySequence(const std::string& keySequence);

		bool IsEnabled() const;
	};
}
