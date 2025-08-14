/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackId.h"

namespace Editors
{
	class ObjectWatcherCallbackId
	{
		friend class ObjectWatcher;

	public:
		ObjectWatcherCallbackId();
		ObjectWatcherCallbackId(void* pObj, Core::CallbackId cid);

		~ObjectWatcherCallbackId();

		bool IsValid() const;

	private:
		void* m_pObj;
		Core::CallbackId m_cid;
	};
}

