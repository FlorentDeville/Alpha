/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Singleton.h"

#include <map>

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Editors
{
	class ObjectWatcher : public Core::Singleton<ObjectWatcher>
	{
	public:

		using WatcherCallbackList = Core::CallbackList<void(Systems::Object*, Systems::FieldDescriptor*)>;
		using WatcherCallback = WatcherCallbackList::Callback;

		ObjectWatcher();
		~ObjectWatcher();

		//Singleton interface
		void Init() override;
		void Shutdown() override;

		void SetFieldValue(Systems::Object* pObj, Systems::FieldDescriptor* pField, void* pValue);

		Core::CallbackId AddWatcher(Systems::Object* pObj, WatcherCallback& callback);
		void RemoveWatcher(Systems::Object* pObj, Core::CallbackId callbackId);

	private:
		std::map<Systems::Object*, WatcherCallbackList> m_watchers;
	};
}
