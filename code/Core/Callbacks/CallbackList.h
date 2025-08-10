/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <functional>
#include <map>

#include "Core/Callbacks/CallbackId.h"

namespace Core
{
	template<typename ReturnType> class CallbackList;

	template<typename ReturnType, typename ...Args> class CallbackList<ReturnType(Args...)>
	{
	public:
		using Callback = std::function<ReturnType(Args...)>;

		CallbackList()
			: m_counter(0)
			, m_callbackMap()
		{}

		~CallbackList()
		{
			m_callbackMap.clear();
		}

		CallbackId Connect(const Callback& callback)
		{
			CallbackId id(m_counter);
			++m_counter;
			m_callbackMap[id] = callback;
			return id;
		}

		void Disconnect(CallbackId id)
		{
			if (!id.IsValid())
				return;

			m_callbackMap.erase(id);
		}

		void Clear()
		{
			m_callbackMap.clear();
		}

		operator bool() const
		{
			return !m_callbackMap.empty();
		}

		void operator ()(Args... args) const
		{
			for (const std::pair<CallbackId,Callback>& callbackPair : m_callbackMap)
			{
				if (callbackPair.second)
					callbackPair.second(args...);
			}
		}

	private:
		uint64_t m_counter;
		std::map<CallbackId, Callback> m_callbackMap;
	};
}
