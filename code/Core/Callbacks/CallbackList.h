/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <functional>
#include <vector>

#include "Core/Callbacks/CallbackId.h"

namespace Core
{
	template<typename ReturnType> class CallbackList;

	template<typename ReturnType, typename ...Args> class CallbackList<ReturnType(Args...)>
	{
	public:
		using Callback = std::function<ReturnType(Args...)>;

		CallbackList()
		{}

		~CallbackList()
		{
			m_callbackArray.clear();
		}

		CallbackId Connect(const Callback& callback)
		{
			CallbackId id(m_callbackArray.size());
			m_callbackArray.push_back(callback);
			return id;
		}

		void Disconnect(CallbackId id)
		{
			if (!id.IsValid())
				return;

			if (id.m_id >= m_callbackArray.size())
				return;

			m_callbackArray[id.m_id] = nullptr;
		}

		operator bool() const
		{
			return !m_callbackArray.empty();
		}

		void operator ()(Args... args) const
		{
			for (const Callback& callback : m_callbackArray)
			{
				if (callback)
					callback(args...);
			}
		}

	private:
		std::vector<Callback> m_callbackArray;
	};
}
