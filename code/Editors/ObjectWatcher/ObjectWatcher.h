/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Singleton.h"

#include <map>
#include <string>

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

		using WatcherCallbackList = Core::CallbackList<void(Systems::Object*, const Systems::FieldDescriptor*)>;
		using WatcherCallback = WatcherCallbackList::Callback;

		ObjectWatcher();
		~ObjectWatcher();

		//Singleton interface
		void Init() override;
		void Shutdown() override;

		void SetFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, const void* pValue);

		void SetArrayFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);

		// Can set a regular field or an element in an array.
		void SetGenericFieldValue(Systems::Object* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);

		Core::CallbackId AddWatcher(Systems::Object* pObj, WatcherCallback& callback);
		void RemoveWatcher(Systems::Object* pObj, Core::CallbackId callbackId);

	private:
		std::map<Systems::Object*, WatcherCallbackList> m_watchers;
	};
}
