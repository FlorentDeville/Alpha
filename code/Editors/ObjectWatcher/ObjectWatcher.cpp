/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Reflection/FieldDescriptor.h"

namespace Editors
{
	ObjectWatcher::ObjectWatcher()
		: Singleton<ObjectWatcher>()
		, m_watchers()
	{ }

	ObjectWatcher::~ObjectWatcher()
	{ }

	void ObjectWatcher::Init()
	{ }

	void ObjectWatcher::Shutdown()
	{
		m_watchers.clear();
	}

	void ObjectWatcher::SetFieldValue(Systems::Object* pObj, Systems::FieldDescriptor* pField, void* pValue)
	{
		void* pFieldPtr = pField->GetDataPtr(pObj);
		memcpy(pFieldPtr, pValue, pField->GetType()->GetSize());
		
		std::map<Systems::Object*, WatcherCallbackList>::const_iterator it = m_watchers.find(pObj);
		if (it == m_watchers.cend())
			return;

		const WatcherCallbackList& callbacks = it->second;
		callbacks(pObj, pField);
	}

	Core::CallbackId ObjectWatcher::AddWatcher(Systems::Object* pObj, WatcherCallback& callback)
	{
		Core::CallbackId id = m_watchers[pObj].Connect(callback);
		return id;
	}

	void ObjectWatcher::RemoveWatcher(Systems::Object* pObj, Core::CallbackId callbackId)
	{
		std::map<Systems::Object*, WatcherCallbackList>::iterator it = m_watchers.find(pObj);
		if (it == m_watchers.end())
			return;

		WatcherCallbackList& callbacks = it->second;
		callbacks.Disconnect(callbackId);
	}
}
