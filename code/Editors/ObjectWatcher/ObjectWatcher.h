/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Singleton.h"

#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

#include <map>
#include <string>

namespace Core
{
	class BaseArray;
}

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

		enum OPERATION
		{
			SET_FIELD,			// set the field to a new value
			SET_ELEMENT,		// the field is an array, set an element of the array to a new value
			ADD_ELEMENT,		// the field is an array, push back a new element to the array
			REMOVE_ELEMENT,		// the field is an array, remove an element from the array

			COUNT
		};

		// Event triggered when a field is modified.
		// void* : the object that was modified
		// const Systems::FieldDescriptor* : the modified field
		// OPERATION : how the field was modified
		// uint32_t : the index of the modified variable in the field. Only usefull for arrays.
		using WatcherCallbackList = Core::CallbackList<void(void*, const Systems::FieldDescriptor*, OPERATION, uint32_t)>;
		using WatcherCallback = WatcherCallbackList::Callback;

		ObjectWatcher();
		~ObjectWatcher();

		//Singleton interface
		void Init() override;
		void Shutdown() override;

		//Operations
		void SetFieldValue(void* pObj, const Systems::FieldDescriptor* pField, const void* pValue);
		void SetArrayFieldValue(void* pObj, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);
		void AddArrayElement(void* pObj, const Systems::FieldDescriptor* pField, const void* pValue);
		void RemoveArrayElement(void* pObj, const Systems::FieldDescriptor* pField, uint32_t index);

		// Generic function, can do any operation
		void ModifyField(void* pObj, const Systems::FieldDescriptor* pField, OPERATION op, uint32_t index, const void* pValue);

		//Manually send modified event. That's used when a field was modified without the object watcher but watchers should be aware of the change.
		void SendFieldModifiedEvent(void* pObj, const Systems::FieldDescriptor* pField, OPERATION op, uint32_t index);

		ObjectWatcherCallbackId AddWatcher(void* pObj, const WatcherCallback& callback);
		ObjectWatcherCallbackId AddWatcher(const void* pObj, const WatcherCallback& callback);
		void RemoveWatcher(ObjectWatcherCallbackId callbackId);

	private:
		std::map<void*, WatcherCallbackList> m_watchers;

		const WatcherCallbackList* FindWatcherCallback(void* pObj) const;

		void Internal_SetArrayFieldValue(Core::BaseArray* pArray, const Systems::FieldDescriptor* pField, uint32_t index, const void* pValue);
	};
}
