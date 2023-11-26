/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Shortcuts/Shortcut.h"

#include "OsWin/Input.h"

namespace Widgets
{
	Shortcut::Shortcut(const std::string& keySequence)
	{
		ParseKeySequence(keySequence);
	}

	Shortcut::~Shortcut()
	{}

	bool Shortcut::Evaluate()
	{
		for (int ii = 0; ii < m_keyCount; ++ii)
		{
			if (!Os::IsKeyDown(m_vkeyCodes[ii]))
				return false;
		}

		if (!m_onActivated)
			return false;

		m_onActivated();
		return true;
	}

	Core::CallbackId Shortcut::OnActivated(const OnActivatedEvent::Callback& callback)
	{
		return m_onActivated.Connect(callback);
	}

	void Shortcut::ParseKeySequence(const std::string& keySequence)
	{
		m_keyCount = 0;
		std::string copyKeySequence = keySequence;
		char* nextToken = nullptr;
		char* token = strtok_s(&copyKeySequence.front(), "+", &nextToken);
		while (token)
		{
			if (strcmp("Ctrl", token) == 0)
			{
				m_vkeyCodes[m_keyCount] = Os::VKeyCodes::Control;
				++m_keyCount;
			}
			else if (strcmp("Alt", token) == 0)
			{
				m_vkeyCodes[m_keyCount] = Os::VKeyCodes::Alt;
				++m_keyCount;
			}
			else if (strcmp("Shift", token) == 0)
			{
				m_vkeyCodes[m_keyCount] = Os::VKeyCodes::Shift;
				++m_keyCount;
			}
			else if (strlen(token) == 1) //single character
			{
				char singleCharacter = token[0];
				if (singleCharacter >= Os::VKeyCodes::Vk_A && singleCharacter <= Os::VKeyCodes::Vk_Z)
				{
					m_vkeyCodes[m_keyCount] = singleCharacter;
					++m_keyCount;
				}
				else if (singleCharacter >= Os::VKeyCodes::Vk_0 && singleCharacter <= Os::VKeyCodes::Vk_9)
				{
					m_vkeyCodes[m_keyCount] = singleCharacter;
					++m_keyCount;
				}
				else
				{
					return;
				}
			}

			token = strtok_s(nullptr, "+", &nextToken);
			if (m_keyCount >= MAX_KEY_COUNT)
				return;
		}
	}
}
